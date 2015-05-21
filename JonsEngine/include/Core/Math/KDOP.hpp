namespace JonsEngine
{
    template <uint32_t MAX_KDOP_PLANES>
    KDOP<MAX_KDOP_PLANES>::KDOP() : mNumPlanes(0)
    {
    }
    
    template <uint32_t MAX_KDOP_PLANES>
    KDOP<MAX_KDOP_PLANES>::~KDOP()
    {
    }


    template <uint32_t MAX_KDOP_PLANES>
    Plane& KDOP<MAX_KDOP_PLANES>::operator[] (const uint32_t index)
    {
        assert(index < mNumPlanes);

        return mPlanes[index];
    }

    template <uint32_t MAX_KDOP_PLANES>
    const Plane& KDOP<MAX_KDOP_PLANES>::operator[] (const uint32_t index) const
    {
        assert(index < mNumPlanes);

        return mPlanes[index];
    }
    
    
    template <uint32_t MAX_KDOP_PLANES>
    KDOP<MAX_KDOP_PLANES>::iterator KDOP<MAX_KDOP_PLANES>::begin()
    {
        return mPlanes.begin();
    }

    template <uint32_t MAX_KDOP_PLANES>
    KDOP<MAX_KDOP_PLANES>::iterator KDOP<MAX_KDOP_PLANES>::end()
    {
        return mPlanes.end();
    }


    template <uint32_t MAX_KDOP_PLANES>
    uint32_t KDOP<MAX_KDOP_PLANES>::TotalPlanes() const
    {
        return mNumPlanes;
    }

    template <uint32_t MAX_KDOP_PLANES>
    void KDOP<MAX_KDOP_PLANES>::AddPlane(const Plane& plane)
    {
        assert(mNumPlanes < MAX_KDOP_PLANES);

        mPlanes[++mNumPlanes] = plane;
    }

    template <uint32_t MAX_KDOP_PLANES>
    void KDOP<MAX_KDOP_PLANES>::Reset()
    {
        mNumPlanes = 0;
    }
}