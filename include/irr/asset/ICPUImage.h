// Copyright (C) 2017- Mateusz 'DevSH' Kielan
// This file is part of the "IrrlichtBAW" engine.
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_CPU_IMAGE_H_INCLUDED__
#define __I_CPU_IMAGE_H_INCLUDED__

#include "irr/core/core.h"

#include "irr/asset/IAsset.h"
#include "irr/asset/ICPUBuffer.h"
#include "irr/asset/IImage.h"
#include "irr/asset/ICPUSampler.h"

namespace irr
{
namespace asset
{

class ICPUImage final : public IImage, public IAsset
{
	public:
		inline static core::smart_refctd_ptr<ICPUImage> create(SCreationParams&& _params)
		{
			if (!validateCreationParameters(_params))
				return nullptr;

			return core::smart_refctd_ptr<ICPUImage>(new ICPUImage(std::move(_params)), core::dont_grab);
		}

        core::smart_refctd_ptr<IAsset> clone(uint32_t _depth = ~0u) const override
        {
            auto par = params;
            auto cp = core::smart_refctd_ptr<ICPUImage>(new ICPUImage(std::move(par)), core::dont_grab);
            clone_common(cp.get());

            cp->regions = regions;
            cp->buffer = (_depth > 0u && buffer) ? core::smart_refctd_ptr_static_cast<ICPUBuffer>(buffer->clone(_depth-1u)) : buffer;

            return cp;
        }

        inline void convertToDummyObject(uint32_t referenceLevelsBelowToConvert=0u) override
        {
            convertToDummyObject_common(referenceLevelsBelowToConvert);

			if (referenceLevelsBelowToConvert)
			if (buffer)
				buffer->convertToDummyObject(referenceLevelsBelowToConvert-1u);

			if (canBeConvertedToDummy())
				regions = nullptr;
        }

		bool canBeRestoredFrom(const IAsset* _other) const override
		{
			if (!IAsset::canBeRestoredFrom(_other))
				return false;

			auto* other = static_cast<const ICPUImage*>(_other);
			if (info != other->info)
				return false;
			if (params != other->params)
				return false;

			return true;
		}

		_IRR_STATIC_INLINE_CONSTEXPR auto AssetType = ET_IMAGE;
		inline IAsset::E_TYPE getAssetType() const override { return AssetType; }

        virtual size_t conservativeSizeEstimate() const override
		{
			assert(regions);
			return sizeof(SCreationParams)+sizeof(void*)+sizeof(SBufferCopy)*regions->size();
		}

		virtual bool validateCopies(const SImageCopy* pRegionsBegin, const SImageCopy* pRegionsEnd, const ICPUImage* src)
		{
			//TODO why is it not const method?
			return validateCopies_template(pRegionsBegin, pRegionsEnd, src);
		}

		inline ICPUBuffer* getBuffer() 
		{
			return buffer.get(); 
		}
		inline const auto* getBuffer() const { return buffer.get(); }

		inline core::SRange<const IImage::SBufferCopy> getRegions() const
		{
			if (regions)
				return {regions->begin(),regions->end()};
			return {nullptr,nullptr};
		}

		inline core::SRange<const IImage::SBufferCopy> getRegions(uint32_t mipLevel) const
		{
			const IImage::SBufferCopy dummy = { 0ull,0u,0u,{static_cast<E_ASPECT_FLAGS>(0u),mipLevel,0u,0u},{},{} };
			auto begin = std::lower_bound(regions->begin(),regions->end(),dummy,mip_order_t());
			auto end = std::upper_bound(regions->begin(),regions->end(),dummy,mip_order_t());
			return {begin,end};
		}

		// `texelCoord=(xTexelPos,yTexelPos,zTexelPos,imageLayer)`
		inline const IImage::SBufferCopy* getRegion(uint32_t mipLevel, const core::vectorSIMDu32& texelCoord) const
		{
			auto mip = getRegions(mipLevel);
			auto found = std::find_if(std::reverse_iterator(mip.end()),std::reverse_iterator(mip.begin()),
				[&texelCoord](const IImage::SBufferCopy& region)
				{ // we can simdify this in the future
					if (region.imageSubresource.baseArrayLayer>texelCoord.w)
						return false;
					if (texelCoord.w>=region.imageSubresource.baseArrayLayer+region.imageSubresource.layerCount)
						return false;

					bool retval = true;
					for (auto i=0; i<3; i++)
					{
						const auto _min = (&region.imageOffset.x)[i];
						const auto _max = _min+(&region.imageExtent.width)[i];
						retval = retval && texelCoord[i]>=_min && texelCoord[i]<_max;
					}
					return retval;
				}
			);
			if (found!=std::reverse_iterator(mip.begin()))
				return &(*found);
			return nullptr;
		}

		//
		inline auto wrapTextureCoordinate(uint32_t mipLevel, const core::vectorSIMDi32& texelCoord, const ISampler::E_TEXTURE_CLAMP wrapModes[3]) const
		{
			auto mipExtent = getMipSize(mipLevel);
			auto mipLastCoord = mipExtent-core::vector3du32_SIMD(1,1,1,1);
			return ICPUSampler::wrapTextureCoordinate(texelCoord,wrapModes,mipExtent,mipLastCoord);
		}


		//
		inline void* getTexelBlockData(const IImage::SBufferCopy* region, const core::vectorSIMDu32& inRegionCoord, core::vectorSIMDu32& outBlockCoord)
		{
			if (isImmutable_debug())
				return nullptr;

			auto localXYZLayerOffset = inRegionCoord/info.getDimension();
			outBlockCoord = inRegionCoord-localXYZLayerOffset*info.getDimension();
			return reinterpret_cast<uint8_t*>(buffer->getPointer())+region->getByteOffset(localXYZLayerOffset,region->getByteStrides(info));
		}
		inline const void* getTexelBlockData(const IImage::SBufferCopy* region, const core::vectorSIMDu32& inRegionCoord, core::vectorSIMDu32& outBlockCoord) const
		{
			return const_cast<typename std::decay<decltype(*this)>::type*>(this)->getTexelBlockData(region,inRegionCoord,outBlockCoord);
		}

		inline void* getTexelBlockData(uint32_t mipLevel, const core::vectorSIMDu32& boundedTexelCoord, core::vectorSIMDu32& outBlockCoord)
		{
			if (isImmutable_debug())
				return nullptr;

			// get region for coord
			const auto* region = getRegion(mipLevel,boundedTexelCoord);
			if (!region)
				return nullptr;
			//
			core::vectorSIMDu32 inRegionCoord(boundedTexelCoord);
			inRegionCoord -= core::vectorSIMDu32(region->imageOffset.x,region->imageOffset.y,region->imageOffset.z,region->imageSubresource.baseArrayLayer);
			return getTexelBlockData(region,inRegionCoord,outBlockCoord);
		}
		inline const void* getTexelBlockData(uint32_t mipLevel, const core::vectorSIMDu32& inRegionCoord, core::vectorSIMDu32& outBlockCoord) const
		{
			return const_cast<typename std::decay<decltype(*this)>::type*>(this)->getTexelBlockData(mipLevel,inRegionCoord,outBlockCoord);
		}


		//! regions will be copied and sorted
		inline bool setBufferAndRegions(core::smart_refctd_ptr<ICPUBuffer>&& _buffer, const core::smart_refctd_dynamic_array<IImage::SBufferCopy>& _regions)
		{
			if (isImmutable_debug())
				return false;
			if (!IImage::validateCopies(_regions->begin(),_regions->end(),_buffer.get()))
				return false;
		
			buffer = std::move(_buffer);
			regions = _regions;
			std::sort(regions->begin(),regions->end(),mip_order_t());
			return true;
		}

    protected:
		ICPUImage(SCreationParams&& _params) : IImage(std::move(_params))
		{
		}

		virtual ~ICPUImage() = default;
		
		
		core::smart_refctd_ptr<asset::ICPUBuffer>				buffer;
		core::smart_refctd_dynamic_array<IImage::SBufferCopy>	regions;

	private:
		struct mip_order_t
		{
			inline bool operator()(const IImage::SBufferCopy& _a, const IImage::SBufferCopy& _b)
			{
				return _a.imageSubresource.mipLevel < _b.imageSubresource.mipLevel;
			}
		};
private:
		void restoreFromDummy_impl(IAsset* _other, uint32_t _levelsBelow) override
		{
			auto* other = static_cast<ICPUImage*>(_other);

			std::swap(regions, other->regions);
		}

};

} // end namespace video
} // end namespace irr

#endif


