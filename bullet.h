/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����Fbullet.h
* �쐬��	�FAT13B284 10 ���}���[��
* �쐬��	�F
********************************************************************************
* �X�V����	�F
*
*******************************************************************************/
/*******************************************************************************
* �C���N���[�h�K�[�h
*******************************************************************************/
#ifndef _BULLET_H_
#define _BULLET_H_

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
class CBullet : public CBillboard
{
protected:
	int m_nLife;									// ����
	OBJTYPE m_ObjType;								// �e�̑���
	D3DXCOLOR m_Col;
	Vector3 m_Vec;

	virtual void UpdateCollision(void);	// �����蔻��
	virtual void DeleteCheak(void);		// ��������
public:
	CBullet(DRAWORDER DrawOrder = DRAWORDER_3DLATE, OBJTYPE ObjType = OBJTYPE_BILLBOARD);
	~CBullet();

	virtual void Init(Vector3 pos, Vector3 vec, D3DXCOLOR col);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);

	static CBullet *Create(Vector3 pos, Vector3 vec, D3DXCOLOR col);
};

class CEnemyBullet : public CBullet
{
protected:
public:
	CEnemyBullet(DRAWORDER DrawOrder = DRAWORDER_3DLATE, OBJTYPE ObjType = OBJTYPE_BILLBOARD);
	~CEnemyBullet(){}

	void Update(void);

	static CEnemyBullet *Create(Vector3 pos, Vector3 vec, D3DXCOLOR col);
};


class CBossBullet : public CBullet
{
protected:
public:
	CBossBullet(DRAWORDER DrawOrder = DRAWORDER_3DLATE, OBJTYPE ObjType = OBJTYPE_BILLBOARD);
	~CBossBullet() {}

	void Update(void);

	static CBossBullet *Create(Vector3 pos, Vector3 vec, D3DXCOLOR col);
};


#endif