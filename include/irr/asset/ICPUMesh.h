#ifndef __IRR_I_CPU_MESH_H_INCLUDED__
#define __IRR_I_CPU_MESH_H_INCLUDED__

#include "irr/asset/IMesh.h"
#include "irr/asset/IAsset.h"
#include "irr/asset/ICPUMeshBuffer.h"
#include "irr/asset/bawformat/blobs/MeshBlob.h"

namespace irr
{
namespace asset
{

class ICPUMesh : public IMesh<ICPUMeshBuffer>, public BlobSerializable, public IAsset
{
	public:
		//! These are not absolute constants, just the most common situation, there may be setups of assets/resources with completely different relationships.
		_IRR_STATIC_INLINE_CONSTEXPR uint32_t MESHBUFFER_HIERARCHYLEVELS_BELOW = 1u;//mesh->meshbuffer
        _IRR_STATIC_INLINE_CONSTEXPR uint32_t PIPELINE_HIERARCHYLEVELS_BELOW = MESHBUFFER_HIERARCHYLEVELS_BELOW+1u;//meshbuffer->pipeline
        _IRR_STATIC_INLINE_CONSTEXPR uint32_t DESC_SET_HIERARCHYLEVELS_BELOW = MESHBUFFER_HIERARCHYLEVELS_BELOW+1u;//meshbuffer->ds
		_IRR_STATIC_INLINE_CONSTEXPR uint32_t IMAGEVIEW_HIERARCHYLEVELS_BELOW = DESC_SET_HIERARCHYLEVELS_BELOW+1u;//ds->imageview
        _IRR_STATIC_INLINE_CONSTEXPR uint32_t IMAGE_HIERARCHYLEVELS_BELOW = IMAGEVIEW_HIERARCHYLEVELS_BELOW+1u;//imageview->image

		//! recalculates the bounding box
		virtual void recalculateBoundingBox(const bool recomputeSubBoxes = false)
		{
			if (isImmutable_debug())
				return;

			core::aabbox3df bbox(FLT_MAX, FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX);

			const auto count = getMeshBufferCount();
			if (count)
			{
				for (uint32_t i=0u; i<count; i++)
				{
					auto* mb = getMeshBuffer(i);
					if (!mb)
						continue;

					if (recomputeSubBoxes)
						mb->recalculateBoundingBox();

					bbox.addInternalBox(mb->getBoundingBox());
				}
			}
			
			setBoundingBox(std::move(bbox));
		}

		void setBoundingBox(const core::aabbox3df& box) override 
		{ 
			if (isImmutable_debug())
				return;

			IMesh<ICPUMeshBuffer>::setBoundingBox(box); 
		}

		bool canBeRestoredFrom(const IAsset* _other) const override
		{
			if (!IAsset::canBeRestoredFrom(_other))
				return false;

			auto other = static_cast<const ICPUMesh*>(_other);
			if (getMeshBufferCount() == other->getMeshBufferCount())
				return false;

			return true;
		}

		//
		virtual E_MESH_TYPE getMeshType() const override { return EMT_NOT_ANIMATED; }

		//! Serializes mesh to blob for *.baw file format.
		/** @param _stackPtr Optional pointer to stack memory to write blob on. If _stackPtr==NULL, sufficient amount of memory will be allocated.
			@param _stackSize Size of stack memory pointed by _stackPtr.
			@returns Pointer to memory on which blob was written.
		*/
		virtual void* serializeToBlob(void* _stackPtr = NULL, const size_t& _stackSize = 0) const override
		{
			return CorrespondingBlobTypeFor<ICPUMesh>::type::createAndTryOnStack(this, _stackPtr, _stackSize);
		}

		virtual void convertToDummyObject(uint32_t referenceLevelsBelowToConvert=0u) override
		{
            convertToDummyObject_common(referenceLevelsBelowToConvert);

			if (referenceLevelsBelowToConvert)
			for (auto i=0u; i<getMeshBufferCount(); i++)
				getMeshBuffer(i)->convertToDummyObject(referenceLevelsBelowToConvert-1u);
		}

		_IRR_STATIC_INLINE_CONSTEXPR auto AssetType = ET_MESH;
		inline E_TYPE getAssetType() const override { return AssetType; }

		virtual size_t conservativeSizeEstimate() const override { return getMeshBufferCount()*sizeof(void*); }

private:
		void restoreFromDummy_impl(IAsset* _other, uint32_t _levelsBelow) override
		{
			auto* other = static_cast<ICPUMesh*>(_other);

			if (_levelsBelow)
			{
				--_levelsBelow;
				for (uint32_t i = 0u; i < getMeshBufferCount(); i++)
					getMeshBuffer(i)->restoreFromDummy(other->getMeshBuffer(i), _levelsBelow);
			}
		}
};

}
}

#endif //__IRR_I_CPU_MESH_H_INCLUDED__
