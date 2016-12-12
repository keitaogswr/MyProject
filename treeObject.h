/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����Fbillboard.h
* �쐬��	�FAT13B284 10 ���}���[��
* �쐬��	�F
********************************************************************************
* �X�V����	�F
*
*******************************************************************************/
/*******************************************************************************
* �C���N���[�h�K�[�h
*******************************************************************************/
#ifndef _TREEOBJECT_H_
#define _TREEOBJECT_H_

/*******************************************************************************
* �C���N���[�h�t�@�C��
*******************************************************************************/

/*******************************************************************************
* �}�N����`
*******************************************************************************/

/*******************************************************************************
* �\����
*******************************************************************************/

/*******************************************************************************
* �O���錾
*******************************************************************************/
class CScene3D;

/*******************************************************************************
* �N���X�錾
*******************************************************************************/
class CTreeObject : public CBillboard
{
public:
	CTreeObject(DRAWORDER DrawOrder = DRAWORDER_3DLATE, OBJTYPE ObjType = OBJTYPE_BILLBOARD);
	~CTreeObject();

	void Init(Vector3 pos, int num);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static CTreeObject *Create(Vector3 pos, int num);
	static void Set(Vector3 pos, int num);

	static int SaveFile(void);
	static int LoadFile(void);

	int GetTreeType(void) { return m_nTreeType; }

	// ���؃I�u�W�F�N�g�̎��
	typedef enum
	{
		TREEOBJ_TYPE_TREE0,
		TREEOBJ_TYPE_TREE1,
		TREEOBJ_TYPE_TREE2,
		TREEOBJ_TYPE_TREE3,
		TREEOBJ_TYPE_TREE4,
		TREEOBJ_TYPE_WEED0,
		TREEOBJ_TYPE_WEED1,
		TREEOBJ_TYPE_WEED2,
		TREEOBJ_TYPE_MAX
	}TREEOBJ_TYPE;

	// ���؃I�u�W�F�N�g�̃p�����[�^
	typedef struct
	{
		int nIdx;
		float width;
		float height;
	}TREEOBJ_PARAM;

	static TREEOBJ_PARAM treeObjParam[TREEOBJ_TYPE_MAX];
	static int m_nTexNum;

	TREEOBJ_TYPE billboardType;		// ���؃I�u�W�F�N�g�̃^�C�v
private:
	int m_nTreeType;
};

#endif