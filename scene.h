/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����Fscene.h
* �쐬��	�FAT13B284 10 ���}���[��
* �쐬��	�F
********************************************************************************
* �X�V����	�F
*
*******************************************************************************/
/*******************************************************************************
* �C���N���[�h�K�[�h
*******************************************************************************/
#ifndef _SCENE_H_
#define _SCENE_H_

/*******************************************************************************
* �C���N���[�h�t�@�C��
*******************************************************************************/

/*******************************************************************************
* ���C�u�����̃����N
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
class Vector3;
class CList;

/*******************************************************************************
* �N���X�錾
*******************************************************************************/
typedef enum
{
	OBJTYPE_NONE,
	OBJTYPE_ENEMY,
	OBJTYPE_PLAYER,
	OBJTYPE_MESHFIELD,
	OBJTYPE_BILLBOARD,
	OBJTYPE_EFFECT,
	OBJTYPE_MAX
}OBJTYPE;

typedef enum
{
	DRAWORDER_BACK,
	DRAWORDER_STENCIL,
	DRAWORDER_3D,
	DRAWORDER_3DLATE,
	DRAWORDER_EFFECT,
	DRAWORDER_2D,
	DRAWORDER_MAX
}DRAWORDER;

// �V�[���N���X
class CScene
{
protected:
	static CScene *m_Top[DRAWORDER_MAX];	// �擪
	static CScene *m_Cur[DRAWORDER_MAX];	// ����

	Vector3 m_Pos;			// �ʒu
	Vector3 m_Rot;			// ��]�l
	bool m_Delete;			// �폜�t���O
	bool m_bPause;			// �|�[�Y�t���O
	bool m_bDraw;			// �`��t���O

	OBJTYPE m_ObjType;
	DRAWORDER m_DrawOrder;
public:
	CScene(DRAWORDER DrawOrder = DRAWORDER_3D, OBJTYPE ObjType = OBJTYPE_NONE);
	~CScene();
	virtual void Init(void) {}
	virtual void Uninit(void) = 0;
	virtual void Update(void) {}
	virtual void Draw(void) = 0;

	static void UninitAll(void);
	static void UpdateAll(void);
	static void DrawAll(void);
	void SetPosition(Vector3 pos);
	void Release(DRAWORDER DrawOrder);
	Vector3 GetPosition(void) { return m_Pos; }
	Vector3 GetRotation(void) { return m_Rot; }

	void SetObjType(OBJTYPE type) { m_ObjType = type; }
	OBJTYPE GetObjType(void) { return m_ObjType; }

	void SetDrawOrder(DRAWORDER type) { m_DrawOrder = type; }
	void SetDeleteFlg(bool flg) { m_Delete = flg; }
	bool GetDeleteFlg(void) { return m_Delete; }
	void SetPauseFlg(bool flg) { m_bPause = flg; }
	void SetDrawFlg(bool flg) { m_bDraw = flg; }

	virtual float GetCollision(void) { return NULL; }
	virtual void SetDamage(int damage) {}
	virtual void SetKeep(int keep) {}
	virtual Vector3 GetTargetPos(void) { return Vector3(0.0f, 0.0f, 0.0f); }
	virtual CScene *GetTarget(void) { return NULL; }

	CScene *m_Prev;			// �O�̃|�C���^
	CScene *m_Next;			// ���̃|�C���^

	static CScene *GetList(DRAWORDER DrawOrder);
	static CScene *GetCur(DRAWORDER DrawOrder);

	CScene *GetNext(void);
};

#endif