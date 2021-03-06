/******************************************************/
/**                                                  **/
/**      SDK/IVModelInfo.h                           **/
/**      |_ @package GO:SX Legit                     **/
/**      |_ @created 2017-12-15                      **/
/**      |_ @author André "vAx" Kalisch              **/
/**                                                  **/
/**      Copyright © 2017 @author                    **/
/**      All rights reserved.                        **/
/**                                                  **/
/******************************************************/

#ifndef SDK_IVModelInfo_h
#define SDK_IVModelInfo_h

#include "Interfaces.h"

class Quaternion;
struct mstudioanimdesc_t;
struct mstudioseqdesc_t;
struct mstudiobodyparts_t;
struct mstudiotexture_t;

enum Hitboxes {
    HITBOX_HEAD,
    HITBOX_NECK,
    HITBOX_PELVIS,
    HITBOX_BODY,
    HITBOX_THORAX,
    HITBOX_CHEST,
    HITBOX_UPPER_CHEST,
    HITBOX_RIGHT_THIGH,
    HITBOX_LEFT_THIGH,
    HITBOX_RIGHT_CALF,
    HITBOX_LEFT_CALF,
    HITBOX_RIGHT_FOOT,
    HITBOX_LEFT_FOOT,
    HITBOX_RIGHT_HAND,
    HITBOX_LEFT_HAND,
    HITBOX_RIGHT_UPPER_ARM,
    HITBOX_RIGHT_FOREARM,
    HITBOX_LEFT_UPPER_ARM,
    HITBOX_LEFT_FOREARM,
    HITBOX_MAX,
    HITBOX_NONE
};

class RadianEuler {
public:
    float x, y, z;
};

class Quaternion {
public:
    float x, y, z, w;
};

struct mstudiobone_t {
    int sznameindex;
    
    inline char *const pszName(void) const {
        return ((char *) this) + sznameindex;
    }
    
    int parent;
    int bonecontroller[6];

    Vector pos;
    Quaternion quat;
    RadianEuler rot;
    Vector posscale;
    Vector rotscale;
    
    matrix3x4_t poseToBone;
    Quaternion qAlignment;
    int flags;
    int proctype;
    int procindex;
    mutable int physicsbone;
    
    inline void *pProcedure() const {
        if (procindex == 0) {
            return NULL;
        } else {
            return (void*)(((unsigned char *) this) + procindex);
        }
    };
    
    int surfacepropidx;
    
    inline char *const pszSurfaceProp(void) const {
        return ((char *) this) + surfacepropidx;
    }
    
    inline int GetSurfaceProp(void) const {
        return surfacepropLookup;
    }
    
    int contents;
    int surfacepropLookup;
    int unused[7];
};

struct mstudiobbox_t {
    int bone;
    int group;
    Vector bbmin;
    Vector bbmax;
    int hitboxnameindex;
    int pad[3];
    float radius;
    int pad2[4];
    
    char *pszHitboxName() {
        if (hitboxnameindex == 0) {
            return NULL;
        }
        
        return ((char *) this) + hitboxnameindex;
    }
};

struct mstudiohitboxset_t {
    int sznameindex;
    
    inline char *const pszName() const {
        return ((char *) this) + sznameindex;
    }
    
    int numhitboxes;
    int hitboxindex;
    
    inline mstudiobbox_t *pHitbox(int i) const {
        return (mstudiobbox_t *) (((unsigned char *) this) + hitboxindex) + i;
    };
};

struct studiohdr_t {
    int id;
    int version;
    int checksum;        // this has to be the same in the phy and vtx files to load!
    char name[64];
    int length;
    
    Vector eyeposition;    // ideal eye position
    Vector illumposition;    // illumination center
    Vector hull_min;        // ideal movement hull size
    Vector hull_max;
    Vector view_bbmin;        // clipping bounding box
    Vector view_bbmax;
    
    int flags;
    int numbones;            // bones
    int boneindex;
    inline mstudiobone_t *pBone(int i) const {
        return (mstudiobone_t *)(((unsigned char * ) this) + boneindex ) + i;
    };
    
    int RemapSeqBone(int iSequence, int iLocalBone) const;    // maps local sequence bone to global bone
    int RemapAnimBone(int iAnim, int iLocalBone) const;        // maps local animations bone to global bone
    int numbonecontrollers;        // bone controllers
    int bonecontrollerindex;
    int numhitboxsets;
    int hitboxsetindex;
    
    // Look up hitbox set by index
    mstudiohitboxset_t* pHitboxSet(int i) const {
        if (i >= 0 && i < numhitboxsets) {
            return (mstudiohitboxset_t*)(((unsigned char * ) this ) +hitboxsetindex ) +i;
        }
        
        return nullptr;
    };
    
    // Calls through to hitbox to determine size of specified set
    inline mstudiobbox_t *pHitbox(int i, int set) const {
        mstudiohitboxset_t const *s = pHitboxSet(set);
        if (!s) {
            return NULL;
        }
        
        return s->pHitbox(i);
    };
    
    // Calls through to set to get hitbox count for set
    inline int iHitboxCount(int set) const {
        mstudiohitboxset_t const *s = pHitboxSet(set);
        if (!s) {
            return 0;
        }
        
        return s->numhitboxes;
    };
    
    // file local animations? and sequences
    //private:
    int numlocalanim;            // animations/poses
    int localanimindex;        // animation descriptions
    int numlocalseq;                // sequences
    int localseqindex;
    
    //public:
    bool SequencesAvailable() const;
    int GetNumSeq() const;
    int iRelativeAnim(int baseseq, int relanim) const;    // maps seq local anim reference to global anim index
    int iRelativeSeq(int baseseq, int relseq) const;        // maps seq local seq reference to global seq index
    
    //private:
    mutable int activitylistversion;    // initialization flag - have the sequences been indexed?
    mutable int eventsindexed;
    
    //public:
    int GetSequenceActivity(int iSequence);
    void SetSequenceActivity(int iSequence, int iActivity);
    int GetActivityListVersion();
    void SetActivityListVersion(int version) const;
    int GetEventListVersion();
    void SetEventListVersion(int version);
    
    // raw textures
    int numtextures;
    int textureindex;
    
    // raw textures search paths
    int numcdtextures;
    int cdtextureindex;
    
    inline char *pCdtexture(int i) const {
        return (((char *) this) + *((int *) (((unsigned char *) this) +cdtextureindex) + i));
    };
    
    // replaceable textures tables
    int numskinref;
    int numskinfamilies;
    int skinindex;
    
    inline short *pSkinref(int i) const {
        return (short *) (((unsigned char *) this) +skinindex) +i;
    };
    int numbodyparts;
    int bodypartindex;
    
    // queryable attachable points
    //private:
    int numlocalattachments;
    int localattachmentindex;
    
    //public:
    int GetNumAttachments() const;
    int GetAttachmentBone(int i);
    // used on my tools in hlmv, not persistant
    void SetAttachmentBone(int iAttachment, int iBone);
    
    // animation node to animation node transition graph
    //private:
    int numlocalnodes;
    int localnodeindex;
    int localnodenameindex;
    
    inline char *pszLocalNodeName(int iNode) const {
        if (iNode >= 0 && iNode < numlocalnodes) {
            return (((char *) this) + *((int *) (((unsigned char *) this) + localnodenameindex) + iNode));
        }
        
        return (char *)"";
    }
    
    inline unsigned char *pLocalTransition(int i) const {
        if (i >= 0 && i < (numlocalnodes * numlocalnodes)) {
            return (unsigned char * )(((unsigned char *) this) + localnodeindex) + i;
        }
        
        return (unsigned char *)"";
    };
    
    //public:
    int EntryNode(int iSequence);
    int ExitNode(int iSequence);
    char *pszNodeName(int iNode);
    int GetTransition(int iFrom, int iTo) const;
    int numflexdesc;
    int flexdescindex;
    int numflexcontrollers;
    int flexcontrollerindex;
    int numflexrules;
    int flexruleindex;
    int numikchains;
    int ikchainindex;
    int nummouths;
    int mouthindex;
    
    //private:
    int numlocalposeparameters;
    int localposeparamindex;
    
    //public:
    int GetNumPoseParameters() const;
    int GetSharedPoseParameter(int iSequence, int iLocalPose) const;
    int surfacepropindex;
    
    inline char *const pszSurfaceProp() const {
        return ((char *) this) + surfacepropindex;
    }
    
    // Key values
    int keyvalueindex;
    int keyvaluesize;
    
    inline const char *KeyValueText() const {
        return keyvaluesize != 0 ? ((char *) this) + keyvalueindex : NULL;
    }
    
    int numlocalikautoplaylocks;
    int localikautoplaylockindex;
    
    int GetNumIKAutoplayLocks() const;
    
    int CountAutoplaySequences() const;
    
    int CopyAutoplaySequences(unsigned short *pOut, int outCount) const;
    
    int GetAutoplayList(unsigned short **pOut) const;
    
    // The collision model mass that jay wanted
    float mass;
    int contents;
    
    // external animations, models, etc.
    int numincludemodels;
    int includemodelindex;
    
    // implementation specific call to get a named model
    const studiohdr_t *FindModel(void **cache, char const *modelname) const;
    
    // implementation specific back pointer to virtual data
    mutable void *virtualModel;
    //virtualmodel_t		GetVirtualModel() const;
    
    // for demand loaded animation blocks
    int szanimblocknameindex;
    
    inline char *const pszAnimBlockName() const {
        return ((char *) this) + szanimblocknameindex;
    }
    
    int numanimblocks;
    int animblockindex;
    mutable void *animblockModel;
    
    unsigned char *GetAnimBlock(int i) const;
    
    int bonetablebynameindex;
    
    inline const unsigned char *GetBoneTableSortedByName() const {
        return (unsigned char *) this + bonetablebynameindex;
    }
    
    // used by tools only that don't cache, but persist mdl's peer data
    // engine uses virtualModel to back link to cache pointers
    void *pVertexBase;
    void *pIndexBase;
    
    // if STUDIOHDR_FLAGS_CONSTANT_DIRECTIONAL_LIGHT_DOT is set,
    // this value is used to calculate directional components of lighting
    // on static props
    unsigned char constdirectionallightdot;
    
    // set during load of mdl data to track *desired* lod configuration (not actual)
    // the *actual* clamped root lod is found in studiohwdata
    // this is stored here as a global store to ensure the staged loading matches the rendering
    unsigned char rootLOD;
    
    // set in the mdl data to specify that lod configuration should only allow first numAllowRootLODs
    // to be set as root LOD:
    //	numAllowedRootLODs = 0	means no restriction, any lod can be set as root lod.
    //	numAllowedRootLODs = N	means that lod0 - lod(N-1) can be set as root lod, but not lodN or lower.
    unsigned char numAllowedRootLODs;
    unsigned char unused[1];
    int unused4; // zero out if version < 47
    int numflexcontrollerui;
    int flexcontrolleruiindex;
    int unused3[2];
    
    // FIXME: Remove when we up the model version. Move all fields of studiohdr2_t into studiohdr_t.
    int studiohdr2index;
    
    // NOTE: No room to add stuff? Up the .mdl file format version
    // [and move all fields in studiohdr2_t into studiohdr_t and kill studiohdr2_t],
    // or add your stuff to studiohdr2_t. See NumSrcBoneTransforms/SrcBoneTransform for the pattern to use.
    int unused2[1];
    
    studiohdr_t() {}
private:
    // No copy constructors allowed
    studiohdr_t(const studiohdr_t &vOther);
    
    friend struct virtualmodel_t;
};

struct studiomeshdata_t {
    int					m_NumGroup;
};

struct studioloddata_t {
    // not needed - this is really the same as studiohwdata_t.m_NumStudioMeshes
    //int					m_NumMeshes;
    studiomeshdata_t	*m_pMeshData; // there are studiohwdata_t.m_NumStudioMeshes of these.
    float				m_SwitchPoint;
    // one of these for each lod since we can switch to simpler materials on lower lods.
    int					numMaterials;
    IMaterial			**ppMaterials; /* will have studiohdr_t.numtextures elements allocated */
    // hack - this needs to go away.
    int					*pMaterialFlags; /* will have studiohdr_t.numtextures elements allocated */

    // For decals on hardware morphing, we must actually do hardware skinning
    // For this to work, we have to hope that the total # of bones used by
    // hw flexed verts is < than the max possible for the dx level we're running under
    int					*m_pHWMorphDecalBoneRemap;
    int					m_nDecalBoneCount;
};

struct studiohwdata_t {
    int					m_RootLOD;	// calced and clamped, nonzero for lod culling
    int					m_NumLODs;
    studioloddata_t		*m_pLODs;
    int					m_NumStudioMeshes;

    inline float LODMetric( float unitSphereSize ) const {
        return (unitSphereSize != 0.0f) ? (100.0f / unitSphereSize) : 0.0f;
    }

    inline int GetLODForMetric( float lodMetric ) const {
        if ( !m_NumLODs )
        return 0;

        // shadow lod is specified on the last lod with a negative switch
        // never consider shadow lod as viable candidate
        int numLODs = (m_pLODs[m_NumLODs-1].m_SwitchPoint < 0.0f) ? m_NumLODs-1 : m_NumLODs;

        for (int i = m_RootLOD; i < numLODs-1; i++) {
            if (m_pLODs[i+1].m_SwitchPoint > lodMetric) {
                return i;
            }
        }
        
        return numLODs-1;
    }
};

class IVModelInfo {
public:
    model_t* GetModel(int index) {
        typedef model_t* (*oGetModel)(IVModelInfo*, int);
        return Interfaces::Function<oGetModel>(this, 2)(this, index);
    }
    
    int GetModelIndex(const char* Filename) {
        typedef int (*oGetModelIndex)(IVModelInfo*, const char*);
        return Interfaces::Function<oGetModelIndex>(this, 3)(this, Filename);
    }
    
    const char* GetModelName(const model_t *model) {
        typedef const char* (*oGetModelName)(IVModelInfo*, const model_t*);
        return Interfaces::Function<oGetModelName>(this, 4)(this, model);
    }
    
    void GetModelMaterials(const model_t* model, int count, IMaterial** ppMaterial) {
        typedef studiohdr_t* (*oGetModelMaterials)(IVModelInfo*, const model_t*, int, IMaterial**);
        Interfaces::Function<oGetModelMaterials>(this, 18)(this, model, count, ppMaterial);
    }
    
    int GetModelMaterialCount(const model_t* model) {
        typedef int (*oGetModelMaterialCount)(IVModelInfo*, const model_t*);
        return Interfaces::Function<oGetModelMaterialCount>(this, 17)(this, model);
    }
    
    studiohdr_t* GetStudioModel(const model_t* model) {
        typedef studiohdr_t* (*oGetStudioModel)(IVModelInfo*, const model_t*);
        return Interfaces::Function<oGetStudioModel>(this, 31)(this, model);
    }
};

extern IVModelInfo* ModelInfo;

#endif /** !SDK_IVModelInfo_h */
