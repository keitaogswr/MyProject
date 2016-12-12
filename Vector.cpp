/*******************************************************************************
* ƒ^ƒCƒgƒ‹–¼F
* ƒtƒ@ƒCƒ‹–¼FVector.cpp
* ì¬Ò	FAT12A244 10 ¬Š}Œ´Œ[‘¾
* ì¬“ú	F
********************************************************************************
* XV—š—ğ	F
*
*******************************************************************************/
/*******************************************************************************
* ƒCƒ“ƒNƒ‹[ƒhƒtƒ@ƒCƒ‹
*******************************************************************************/
#include "Vector.h"

// ƒRƒ“ƒXƒgƒ‰ƒNƒ^
Vector3::Vector3( float x, float y, float z )
{

	this->x = x;
	this->y = y;
	this->z = z;

}

// ‰ÁZ
Vector3 Vector3::operator += ( const Vector3 Vec3 )
{

	this->x += Vec3.x;
	this->y += Vec3.y;
	this->z += Vec3.z;

	return *this;
}

// ‰ÁZ‚µ‚Ä‘ã“ü
Vector3 Vector3::operator + ( const Vector3 Vec3 )
{
	Vector3 vector;

	vector.x = this->x + Vec3.x;
	vector.y = this->y + Vec3.y;
	vector.z = this->z + Vec3.z;

	return vector;
}

// Œ¸Z
Vector3 Vector3::operator -= ( const Vector3 Vec3 )
{

	this->x -= Vec3.x;
	this->y -= Vec3.y;
	this->z -= Vec3.z;

	return *this;
}

// Œ¸Z‚µ‚Ä‘ã“ü
Vector3 Vector3::operator - ( const Vector3 Vec3 )
{
	Vector3 vector;

	vector.x = this->x - Vec3.x;
	vector.y = this->y - Vec3.y;
	vector.z = this->z - Vec3.z;

	return vector;
}

// ‰ÁZ( D3DXVECTOR3 )
Vector3 Vector3::operator += ( const D3DXVECTOR3 Vec3 )
{

	this->x += Vec3.x;
	this->y += Vec3.y;
	this->z += Vec3.z;

	return *this;
}

// ‰ÁZ‚µ‚Ä‘ã“ü( D3DXVECTOR3 )
Vector3 Vector3::operator + ( const D3DXVECTOR3 Vec3 )
{
	Vector3 vector;

	vector.x = this->x + Vec3.x;
	vector.y = this->y + Vec3.y;
	vector.z = this->z + Vec3.z;

	return vector;
}

// Œ¸Z( D3DXVECTOR3 )
Vector3 Vector3::operator -= ( const D3DXVECTOR3 Vec3 )
{

	this->x -= Vec3.x;
	this->y -= Vec3.y;
	this->z -= Vec3.z;

	return *this;
}

// Œ¸Z‚µ‚Ä‘ã“ü( D3DXVECTOR3 )
Vector3 Vector3::operator - ( const D3DXVECTOR3 Vec3 )
{
	Vector3 vector;

	vector.x = this->x - Vec3.x;
	vector.y = this->y - Vec3.y;
	vector.z = this->z - Vec3.z;

	return vector;
}

// æZ
Vector3 Vector3::operator *= ( float f )
{

	this->x *= f;
	this->y *= f;
	this->z *= f;

	return *this;
}

// æZ‚µ‚Ä‘ã“ü
Vector3 Vector3::operator * ( float f )
{
	Vector3 vector;

	vector.x = this->x * f;
	vector.y = this->y * f;
	vector.z = this->z * f;

	return vector;
}

// œZ
Vector3 Vector3::operator /= ( float f )
{
	this->x /= f;
	this->y /= f;
	this->z /= f;

	return *this;
}

// œZ‚µ‚Ä‘ã“ü
Vector3 Vector3::operator / ( float f )
{
	Vector3 vector;

	vector.x = this->x / f;
	vector.y = this->y / f;
	vector.z = this->z / f;

	return vector;
}

// ³‹K‰»
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
// Desc : ’·‚³‚ğ‹‚ß‚é
//-----------------------------------------------------------------------
float Vector3::Length(void)
{
	float length;

	length = sqrtf(this->x * this->x + this->y * this->y + this->z * this->z);

	return length;
}

// Vector2
// ƒRƒ“ƒXƒgƒ‰ƒNƒ^
Vector2::Vector2(float x, float y)
{

	this->x = x;
	this->y = y;

}

// ‰ÁZ
Vector2 Vector2::operator += (const Vector2 Vec2)
{

	this->x += Vec2.x;
	this->y += Vec2.y;

	return *this;
}

// ‰ÁZ‚µ‚Ä‘ã“ü
Vector2 Vector2::operator + (const Vector2 Vec2)
{
	Vector2 vector;

	vector.x = this->x + Vec2.x;
	vector.y = this->y + Vec2.y;

	return vector;
}

// Œ¸Z
Vector2 Vector2::operator -= (const Vector2 Vec2)
{

	this->x -= Vec2.x;
	this->y -= Vec2.y;

	return *this;
}

// Œ¸Z‚µ‚Ä‘ã“ü
Vector2 Vector2::operator - (const Vector2 Vec2)
{
	Vector2 vector;

	vector.x = this->x - Vec2.x;
	vector.y = this->y - Vec2.y;

	return vector;
}

// ‰ÁZ( D3DXVECTOR3 )
Vector2 Vector2::operator += (const D3DXVECTOR2 Vec2)
{

	this->x += Vec2.x;
	this->y += Vec2.y;

	return *this;
}

// ‰ÁZ‚µ‚Ä‘ã“ü( D3DXVECTOR3 )
Vector2 Vector2::operator + (const D3DXVECTOR2 Vec2)
{
	Vector2 vector;

	vector.x = this->x + Vec2.x;
	vector.y = this->y + Vec2.y;

	return vector;
}

// Œ¸Z( D3DXVECTOR3 )
Vector2 Vector2::operator -= (const D3DXVECTOR2 Vec2)
{

	this->x -= Vec2.x;
	this->y -= Vec2.y;

	return *this;
}

// Œ¸Z‚µ‚Ä‘ã“ü( D3DXVECTOR3 )
Vector2 Vector2::operator - (const D3DXVECTOR2 Vec2)
{
	Vector2 vector;

	vector.x = this->x - Vec2.x;
	vector.y = this->y - Vec2.y;

	return vector;
}


// æZ
Vector2 Vector2::operator *= (float f)
{

	this->x *= f;
	this->y *= f;

	return *this;
}

// æZ‚µ‚Ä‘ã“ü
Vector2 Vector2::operator * (float f)
{
	Vector2 vector;

	vector.x = this->x * f;
	vector.y = this->y * f;

	return vector;
}

// œZ
Vector2 Vector2::operator /= (float f)
{
	this->x /= f;
	this->y /= f;

	return *this;
}

// œZ‚µ‚Ä‘ã“ü
Vector2 Vector2::operator / (float f)
{
	Vector2 vector;

	vector.x = this->x / f;
	vector.y = this->y / f;

	return vector;
}

// ³‹K‰»
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
// Desc : ’·‚³‚ğ‹‚ß‚é
//-----------------------------------------------------------------------
float Vector2::Length(void)
{
	float length;

	length = sqrtf(this->x * this->x + this->y * this->y);

	return length;
}