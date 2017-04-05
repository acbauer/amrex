#ifndef BL_AMRMESH_H_
#define BL_AMRMESH_H_

#include <AMReX_Array.H>
#include <AMReX_RealBox.H>
#include <AMReX_IntVect.H>
#include <AMReX_Geometry.H>
#include <AMReX_DistributionMapping.H>
#include <AMReX_BoxArray.H>
#include <AMReX_TagBox.H>

namespace amrex {

class AmrMesh
{
public:
    AmrMesh ();
    AmrMesh (const RealBox* rb, int max_level_in, const Array<int>& n_cell_in, int coord);

    AmrMesh (const AmrMesh& rhs) = delete;
    AmrMesh& operator= (const AmrMesh& rhs) = delete;

    virtual ~AmrMesh ();

    int Verbose () const { return verbose; }

    //! Return the max level
    int maxLevel () const { return max_level; }

    //! Return the finest level
    int finestLevel () const { return finest_level; }

    //! Return the refinement ratio for level lev
    IntVect refRatio (int lev) const { return ref_ratio[lev]; }

    //! Return the maximum refinement ratio in any direction.
    int MaxRefRatio (int lev) const;

    //! Return refinement ratios between all levels.
    const Array<IntVect>& refRatio () const { return ref_ratio; }

    const Array<Geometry>& Geom () const { return geom; }
    const Array<DistributionMapping>& DistributionMap () const { return dmap; }
    const Array<BoxArray>& boxArray () const { return grids; }

    const Geometry& Geom (int lev) const { return geom[lev]; }
    const DistributionMapping& DistributionMap (int lev) const { return dmap[lev]; }
    const BoxArray& boxArray (int lev) const { return grids[lev]; }

    Array<Geometry>& Geom () { return geom; }
    Geometry& Geom (int lev) { return geom[lev]; }

    void SetDistributionMap (int lev, const DistributionMapping& dmap_in);
    void SetBoxArray (int lev, const BoxArray& ba_in);

    void ClearDistributionMap (int lev);
    void ClearBoxArray (int lev);

    //! Return the number of buffer cells in error estimator.
    int nErrorBuf (int lev) const { return n_error_buf[lev]; }

    //! Return the minimum allowable grid efficiency.
    Real gridEff () const { return grid_eff; }

    //! Return the number of cells to define proper nesting 
    int nProper () const { return n_proper; }

    //! Return the blocking factor at level lev
    int blockingFactor (int lev) const { return blocking_factor[lev]; }

    //! Return the largest allowable grid.
    int maxGridSize (int lev) const { return max_grid_size[lev]; }

    bool LevelDefined (int lev);

    //! Should we keep the coarser grids fixed (and not regrid those levels) at all?
    bool useFixedCoarseGrids () const { return use_fixed_coarse_grids; }

    //! Up to what level should we keep the coarser grids fixed (and not regrid those levels)?
    int useFixedUpToLevel () const { return use_fixed_upto_level; }

    //! "Try" to chop up grids so that the number of boxes in the BoxArray is greater than the target_size.
    void ChopGrids (int lev, BoxArray& ba, int target_size) const;

    //! Make a level 0 grids covering the whole domain.  It does NOT install the new grids.
    BoxArray MakeBaseGrids () const;

    /**
    * \brief Make new grids based on error estimates.  This functin
    * expects that valid BoxArrays exist in this->grids from level
    * lbase to level this->finest_level (the current finest level).
    * new_grids.  On return, the new finest level is stored in
    * new_finest, and the new grids are stored in new_grids from Array
    * element lbase+1 to new_finest_level (unless fixed grids are
    * used).  Note that this function adds at most one more level to
    * the existing levels, and it may remove all levels above the base
    * level.  This function does not change the value of
    * this->finest_level, nor does it modifies any BoxArrays stored in
    * this->grids.  It also does not modify new_grids's elements
    * outside the range [lbase+1,new_finest_level].
    */
    void MakeNewGrids (int lbase, Real time, int& new_finest, Array<BoxArray>& new_grids);

    //! Tagging cells for refinement
    virtual void ErrorEst (int lev, TagBoxArray& tags, Real time, int ngrow) {}

    virtual BoxArray GetAreaNotToTag (int lev) { return BoxArray(); }

    virtual void ManualTagsPlacement (int lev, TagBoxArray& tags, Array<IntVect>& bf_lev) {}

    static void Initialize ();
    static void Finalize ();

protected:
    
    int              verbose;
    int              max_level;    // Maximum allowed level.
    Array<IntVect>   ref_ratio;    // Refinement ratios [0:finest_level-1]

    int              finest_level; // Current finest level.

    Array<int>       n_error_buf;     // Buffer cells around each tagged cell.
    Array<int>       blocking_factor; // Blocking factor in grid generation (by level).
    Array<int>       max_grid_size;   // Maximum allowable grid size (by level).
    Real             grid_eff;        // Grid efficiency.
    int              n_proper;        // # cells required for proper nesting.

    bool use_fixed_coarse_grids;
    int  use_fixed_upto_level;
    bool refine_grid_layout; // chop up grids to have the number of grids no less the number of procs

    Array<Geometry>            geom;
    Array<DistributionMapping> dmap;
    Array<BoxArray>            grids;    

private:
    void InitAmrMesh (int max_level_in, const Array<int>& n_cell_in);

    static void ProjPeriodic (BoxList& bd, const Geometry& geom);
};

}

#endif
