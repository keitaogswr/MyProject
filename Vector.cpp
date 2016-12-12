/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����FVector.cpp
* �쐬��	�FAT12A244 10 ���}���[��
* �쐬��	�F
********************************************************************************
* �X�V����	�F
*
*******************************************************************************/
/*******************************************************************************
* �C���N���[�h�t�@�C��
*******************************************************************************/
#include "Vector.h"

// �R���X�g���N�^
Vector3::Vector3( float x, float y, float z )
{

	this->x = x;
	this->y = y;
	this->z = z;

}

// ���Z
Vector3 Vector3::operator += ( const Vector3 Vec3 )
{

	this->x += Vec3.x;
	this->y += Vec3.y;
	this->z += Vec3.z;

	return *this;
}

// ���Z���đ��
Vector3 Vector3::operator + ( const Vector3 Vec3 )
{
	Vector3 vector;

	vector.x = this->x + Vec3.x;
	vector.y = this->y + Vec3.y;
	vector.z = this->z + Vec3.z;

	return vector;
}

// ���Z
Vector3 Vector3::operator -= ( const Vector3 Vec3 )
{

	this->x -= Vec3.x;
	this->y -= Vec3.y;
	this->z -= Vec3.z;

	return *this;
}

// ���Z���đ��
Vector3 Vector3::operator - ( const Vector3 Vec3 )
{
	Vector3 vector;

	vector.x = this->x - Vec3.x;
	vector.y = this->y - Vec3.y;
	vector.z = this->z - Vec3.z;

	return vector;
}

// ���Z( D3DXVECTOR3 )
Vector3 Vector3::operator += ( const D3DXVECTOR3 Vec3 )
{

	this->x += Vec3.x;
	this->y += Vec3.y;
	this->z += Vec3.z;

	return *this;
}

// ���Z���đ��( D3DXVECTOR3 )
Vector3 Vector3::operator + ( const D3DXVECTOR3 Vec3 )
{
	Vector3 vector;

	vector.x = this->x + Vec3.x;
	vector.y = this->y + Vec3.y;
	vector.z = this->z + Vec3.z;

	return vector;
}

// ���Z( D3DXVECTOR3 )
Vector3 Vector3::operator -= ( const D3DXVECTOR3 Vec3 )
{

	this->x -= Vec3.x;
	this->y -= Vec3.y;
	this->z -= Vec3.z;

	return *this;
}

// ���Z���đ��( D3DXVECTOR3 )
Vector3 Vector3::operator - ( const D3DXVECTOR3 Vec3 )
{
	Vector3 vector;

	vector.x = this->x - Vec3.x;
	vector.y = this->y - Vec3.y;
	vector.z = this->z - Vec3.z;

	return vector;
}

// ��Z
Vector3 Vector3::operator *= ( float f )
{

	this->x *= f;
	this->y *= f;
	this->z *= f;

	return *this;
}

// ��Z���đ��
Vector3 Vector3::operator * ( float f )
{
	Vector3 vector;

	vector.x = this->x * f;
	vector.y = this->y * f;
	vector.z = this->z * f;

	return vector;
}

// ���Z
Vector3 Vector3::operator /= ( float f )
{
	this->x /= f;
	this->y /= f;
	this->z /= f;

	return *this;
}

// ���Z���đ��
Vector3 Vector3::operator / ( float f )
{
	Vector3 vector;

	vector.x = this->x / f;
	vector.y = this->y / f;
	vector.z = this->z / f;

	return vector;
}

// ���K��
Vector3 Vector3::Normalize( void )
{
	float length;

	length = sqrtf( this->x * this->x + this->y * this->y + this->z * this->z );

	if( length > 0 )
	{
		this->x /= length;
		this->y /= length;
		this->z /= length;
	}

	return *this;
}

//------------------------------------------------------------------------
// operator float*
//------------------------------------------------------------------------
Vector3::operator float *()
{
	return (float*)&x;
}

//------------------------------------------------------------------------
// operator const float*
//------------------------------------------------------------------------
Vector3::operator const float *() const
{
	return (const float*)&x;
}

//-----------------------------------------------------------------------
// Name : Length()
// Desc : ���������߂�
//-----------------------------------------------------------------------
float Vector3::Length(void)
{
	float length;

	length = sqrtf(this->x * this->x + this->y * this->y + this->z * this->z);

	return length;
}

// Vector2
// �R���X�g���N�^
Vector2::Vector2(float x, float y)
{

	this->x = x;
	this->y = y;

}

// ���Z
Vector2 Vector2::operator += (const Vector2 Vec2)
{

	this->x += Vec2.x;
	this->y += Vec2.y;

	return *this;
}

// ���Z���đ��
Vector2 Vector2::operator + (const Vector2 Vec2)
{
	Vector2 vector;

	vector.x = this->x + Vec2.x;
	vector.y = this->y + Vec2.y;

	return vector;
}

// ���Z
Vector2 Vector2::operator -= (const Vector2 Vec2)
{

	this->x -= Vec2.x;
	this->y -= Vec2.y;

	return *this;
}

// ���Z���đ��
Vector2 Vector2::operator - (const Vector2 Vec2)
{
	Vector2 vector;

	vector.x = this->x - Vec2.x;
	vector.y = this->y - Vec2.y;

	return vector;
}

// ���Z( D3DXVECTOR3 )
Vector2 Vector2::operator += (const D3DXVECTOR2 Vec2)
{

	this->x += Vec2.x;
	this->y += Vec2.y;

	return *this;
}

// ���Z���đ��( D3DXVECTOR3 )
Vector2 Vector2::operator + (const D3DXVECTOR2 Vec2)
{
	Vector2 vector;

	vector.x = this->x + Vec2.x;
	vector.y = this->y + Vec2.y;

	return vector;
}

// ���Z( D3DXVECTOR3 )
Vector2 Vector2::operator -= (const D3DXVECTOR2 Vec2)
{

	this->x -= Vec2.x;
	this->y -= Vec2.y;

	return *this;
}

// ���Z���đ��( D3DXVECTOR3 )
Vector2 Vector2::operator - (const D3DXVECTOR2 Vec2)
{
	Vector2 vector;

	vector.x = this->x - Vec2.x;
	vector.y = this->y - Vec2.y;

	return vector;
}


// ��Z
Vector2 Vector2::operator *= (float f)
{

	this->x *= f;
	this->y *= f;

	return *this;
}

// ��Z���đ��
Vector2 Vector2::operator * (float f)
{
	Vector2 vector;

	vector.x = this->x * f;
	vector.y = this->y * f;

	return vector;
}

// ���Z
Vector2 Vector2::operator /= (float f)
{
	this->x /= f;
	this->y /= f;

	return *this;
}

// ���Z���đ��
Vector2 Vector2::operator / (float f)
{
	Vector2 vector;

	vector.x = this->x / f;
	vector.y = this->y / f;

	return vector;
}

// ���K��
Vector2 Vector2::Normalize(void)
{
	float length;

	length = sqrtf(this->x * this->x + this->y * this->y);

	this->x /= length;
	this->y /= length;

	return *this;
}

//------------------------------------------------------------------------
// operator float*
//------------------------------------------------------------------------
Vector2::operator float *()
{
	return (float*)&x;
}

//------------------------------------------------------------------------
// operator const float*
//------------------------------------------------------------------------
Vector2::operator const float *() const
{
	return (const float*)&x;
}

//-----------------------------------------------------------------------
// Name : Length()
// Desc : ���������߂�
//-----------------------------------------------------------------------
float Vector2::Length(void)
{
	float length;

	length = sqrtf(this->x * this->x + this->y * this->y);

	return length;
}