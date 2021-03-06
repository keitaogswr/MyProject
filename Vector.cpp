/*******************************************************************************
* タイトル名：
* ファイル名：Vector.cpp
* 作成者	：AT12A244 10 小笠原啓太
* 作成日	：
********************************************************************************
* 更新履歴	：
*
*******************************************************************************/
/*******************************************************************************
* インクルードファイル
*******************************************************************************/
#include "Vector.h"

// コンストラクタ
Vector3::Vector3( float x, float y, float z )
{

	this->x = x;
	this->y = y;
	this->z = z;

}

// 加算
Vector3 Vector3::operator += ( const Vector3 Vec3 )
{

	this->x += Vec3.x;
	this->y += Vec3.y;
	this->z += Vec3.z;

	return *this;
}

// 加算して代入
Vector3 Vector3::operator + ( const Vector3 Vec3 )
{
	Vector3 vector;

	vector.x = this->x + Vec3.x;
	vector.y = this->y + Vec3.y;
	vector.z = this->z + Vec3.z;

	return vector;
}

// 減算
Vector3 Vector3::operator -= ( const Vector3 Vec3 )
{

	this->x -= Vec3.x;
	this->y -= Vec3.y;
	this->z -= Vec3.z;

	return *this;
}

// 減算して代入
Vector3 Vector3::operator - ( const Vector3 Vec3 )
{
	Vector3 vector;

	vector.x = this->x - Vec3.x;
	vector.y = this->y - Vec3.y;
	vector.z = this->z - Vec3.z;

	return vector;
}

// 加算( D3DXVECTOR3 )
Vector3 Vector3::operator += ( const D3DXVECTOR3 Vec3 )
{

	this->x += Vec3.x;
	this->y += Vec3.y;
	this->z += Vec3.z;

	return *this;
}

// 加算して代入( D3DXVECTOR3 )
Vector3 Vector3::operator + ( const D3DXVECTOR3 Vec3 )
{
	Vector3 vector;

	vector.x = this->x + Vec3.x;
	vector.y = this->y + Vec3.y;
	vector.z = this->z + Vec3.z;

	return vector;
}

// 減算( D3DXVECTOR3 )
Vector3 Vector3::operator -= ( const D3DXVECTOR3 Vec3 )
{

	this->x -= Vec3.x;
	this->y -= Vec3.y;
	this->z -= Vec3.z;

	return *this;
}

// 減算して代入( D3DXVECTOR3 )
Vector3 Vector3::operator - ( const D3DXVECTOR3 Vec3 )
{
	Vector3 vector;

	vector.x = this->x - Vec3.x;
	vector.y = this->y - Vec3.y;
	vector.z = this->z - Vec3.z;

	return vector;
}

// 乗算
Vector3 Vector3::operator *= ( float f )
{

	this->x *= f;
	this->y *= f;
	this->z *= f;

	return *this;
}

// 乗算して代入
Vector3 Vector3::operator * ( float f )
{
	Vector3 vector;

	vector.x = this->x * f;
	vector.y = this->y * f;
	vector.z = this->z * f;

	return vector;
}

// 除算
Vector3 Vector3::operator /= ( float f )
{
	this->x /= f;
	this->y /= f;
	this->z /= f;

	return *this;
}

// 除算して代入
Vector3 Vector3::operator / ( float f )
{
	Vector3 vector;

	vector.x = this->x / f;
	vector.y = this->y / f;
	vector.z = this->z / f;

	return vector;
}

// 正規化
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
// Desc : 長さを求める
//-----------------------------------------------------------------------
float Vector3::Length(void)
{
	float length;

	length = sqrtf(this->x * this->x + this->y * this->y + this->z * this->z);

	return length;
}

//-----------------------------------------------------------------------
// Name : Length()
// Desc : 長さを求める
//-----------------------------------------------------------------------
float Vector3::LengthSq(void)
{
	float length;

	length = this->x * this->x + this->y * this->y + this->z * this->z;

	return length;
}

// Vector2
// コンストラクタ
Vector2::Vector2(float x, float y)
{

	this->x = x;
	this->y = y;

}

// 加算
Vector2 Vector2::operator += (const Vector2 Vec2)
{

	this->x += Vec2.x;
	this->y += Vec2.y;

	return *this;
}

// 加算して代入
Vector2 Vector2::operator + (const Vector2 Vec2)
{
	Vector2 vector;

	vector.x = this->x + Vec2.x;
	vector.y = this->y + Vec2.y;

	return vector;
}

// 減算
Vector2 Vector2::operator -= (const Vector2 Vec2)
{

	this->x -= Vec2.x;
	this->y -= Vec2.y;

	return *this;
}

// 減算して代入
Vector2 Vector2::operator - (const Vector2 Vec2)
{
	Vector2 vector;

	vector.x = this->x - Vec2.x;
	vector.y = this->y - Vec2.y;

	return vector;
}

// 加算( D3DXVECTOR3 )
Vector2 Vector2::operator += (const D3DXVECTOR2 Vec2)
{

	this->x += Vec2.x;
	this->y += Vec2.y;

	return *this;
}

// 加算して代入( D3DXVECTOR3 )
Vector2 Vector2::operator + (const D3DXVECTOR2 Vec2)
{
	Vector2 vector;

	vector.x = this->x + Vec2.x;
	vector.y = this->y + Vec2.y;

	return vector;
}

// 減算( D3DXVECTOR3 )
Vector2 Vector2::operator -= (const D3DXVECTOR2 Vec2)
{

	this->x -= Vec2.x;
	this->y -= Vec2.y;

	return *this;
}

// 減算して代入( D3DXVECTOR3 )
Vector2 Vector2::operator - (const D3DXVECTOR2 Vec2)
{
	Vector2 vector;

	vector.x = this->x - Vec2.x;
	vector.y = this->y - Vec2.y;

	return vector;
}


// 乗算
Vector2 Vector2::operator *= (float f)
{

	this->x *= f;
	this->y *= f;

	return *this;
}

// 乗算して代入
Vector2 Vector2::operator * (float f)
{
	Vector2 vector;

	vector.x = this->x * f;
	vector.y = this->y * f;

	return vector;
}

// 除算
Vector2 Vector2::operator /= (float f)
{
	this->x /= f;
	this->y /= f;

	return *this;
}

// 除算して代入
Vector2 Vector2::operator / (float f)
{
	Vector2 vector;

	vector.x = this->x / f;
	vector.y = this->y / f;

	return vector;
}

// 正規化
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
// Desc : 長さを求める
//-----------------------------------------------------------------------
float Vector2::Length(void)
{
	float length;

	length = sqrtf(this->x * this->x + this->y * this->y);

	return length;
}

//-----------------------------------------------------------------------
// Name : Length()
// Desc : 長さを求める
//-----------------------------------------------------------------------
float Vector2::LengthSq(void)
{
	float length;

	length = this->x * this->x + this->y * this->y;

	return length;
}