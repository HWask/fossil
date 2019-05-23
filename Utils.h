#pragma once
#include <Windows.h>
#include <d3dx9.h>
#include <cstdio>
#include <string>
#include <iomanip>
#include <sstream>

#define _USE_MATH_DEFINES
#include <cmath>

#include "Vector.h"
#include "matrix3x4_t.h"


class Utils
{
private:
	bool static DataCompare(const BYTE* Data, const BYTE* HexMask, const char* MatchMask)
	{
		for (; *MatchMask; ++MatchMask, ++Data, ++HexMask)
		{
			if (*MatchMask == 'x' && *Data != *HexMask)
			{
				return false;
			}
		}
		return (*MatchMask) == 0;
	}
public:
	static void ColorToRGB(DWORD color, int* r, int* g, int* b)
	{
		*b = color % 256; color /= 256;
		*g = color % 256; color /= 256;
		*r = color % 256;
	}

	static std::string to_string_with_precision(float val)
	{
		std::ostringstream out;
		out << std::setprecision(4) << val;
		return out.str();
	}

	static void VectorAngles(Vector& forward, Vector &angles)
	{
		float tmp, yaw, pitch;

		if (forward.y == 0 && forward.x == 0)
		{
			yaw = 0;
			if (forward.z > 0)
				pitch = 270;
			else
				pitch = 90;
		}
		else
		{
			yaw = (atan2(forward.y, forward.x) * 180 / M_PI);
			if (yaw < 0)
				yaw += 360;

			tmp = sqrt(forward.x * forward.x + forward.y * forward.y);
			pitch = (atan2(-forward.z, tmp) * 180 / M_PI);
			if (pitch < 0)
				pitch += 360;
		}

		angles.x = pitch;
		angles.y = yaw;
		angles.z = 0;
	}

	static void AngleVectors(Vector &angles, Vector& forward)
	{
		float sp, sy, cp, cy;
		float pitch = D3DXToRadian(angles.x);
		float yaw = D3DXToRadian(angles.y);

		sp = sin(pitch);
		cp = cos(pitch);

		sy = sin(yaw);
		cy = cos(yaw);

		forward.x = cp*cy;
		forward.y = cp*sy;
		forward.z = -sp;
	}

	static bool str2Bool(std::string s)
	{
		return s == "false" ? false : true;
	}

	static int str2Int(std::string s)
	{
		return std::stoi(s);
	}

	static float str2float(std::string s)
	{
		return std::stof(s);
	}

	static std::string float2str(float f)
	{
		return std::to_string(f);
	}

	static std::string int2str(int i)
	{
		return std::to_string(i);
	}

	static std::string bool2str(bool b)
	{
		return b ? "true" : "false";
	}

	static void AngleVectors(Vector &angles, Vector &forward, Vector &right, Vector &up)
	{
		float sr, sp, sy, cr, cp, cy;

		sy = sin(D3DXToRadian(angles.y));
		sp = sin(D3DXToRadian(angles.x));
		sr = sin(D3DXToRadian(angles.z));

		cy = cos(D3DXToRadian(angles.y));
		cp = cos(D3DXToRadian(angles.x));
		cr = cos(D3DXToRadian(angles.z));

		forward.x = cp*cy;
		forward.y = cp*sy;
		forward.z = -sp;

		right.x = (-1 * sr*sp*cy + -1 * cr*-sy);
		right.y = (-1 * sr*sp*sy + -1 * cr*cy);
		right.z = -1 * sr*cp;

		up.x = (cr*sp*cy + -sr*-sy);
		up.y = (cr*sp*sy + -sr*cy);
		up.z = cr*cp;
	}

	static std::wstring asciiToUnicode(char* str)
	{
		auto len = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
		std::wstring buf;
		buf.resize(len);
		MultiByteToWideChar(CP_UTF8, 0, str, -1, (wchar_t*)buf.c_str(), len);

		return buf;
	}

	static std::wstring asciiToUnicode(std::string str)
	{
		auto len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
		std::wstring buf;
		buf.resize(len);
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, (wchar_t*)buf.c_str(), len);

		return buf;
	}

	DWORD static FindPattern(DWORD Address, DWORD Len, BYTE* HexMask, char* MatchMask)
	{
		for (DWORD i = 0; i < Len; i++)
		{
			if (DataCompare((BYTE*)(Address + i), HexMask, MatchMask))
			{
				return (DWORD)(Address + i);
			}
		}

		return 0;
	}

	template<typename Function> Function static vfunc(PVOID Base, DWORD Index)
	{
		PDWORD* VTablePointer = (PDWORD*)Base;
		PDWORD VTableFunctionBase = *VTablePointer;
		DWORD dwAddress = VTableFunctionBase[Index];
		return (Function)(dwAddress);
	}

	HRESULT static GenerateShader(IDirect3DDevice9* Device, IDirect3DPixelShader9** Shader, float r, float g, float b)
	{
		char ShaderAsmBuf[256];
		ID3DXBuffer* ShaderBuf = NULL;
		sprintf_s(ShaderAsmBuf, "ps_3_0\ndef c0, %f, %f, %f, %f\nmov oC0,c0", r, g, b, 1.0f);
		if (FAILED(D3DXAssembleShader(ShaderAsmBuf, (strlen(ShaderAsmBuf) + 1), NULL, NULL, 0, &ShaderBuf, NULL)))
		{
			return E_FAIL;
		}

		return Device->CreatePixelShader((const DWORD*)ShaderBuf->GetBufferPointer(), Shader);
	}

	bool static ScreenTransform(const Vector& point, Vector& screen, D3DXMATRIX worldToScreen, int width, int height)
	{
		screen.x = worldToScreen.m[0][0] * point.x + worldToScreen.m[0][1] * point.y + worldToScreen.m[0][2] * point.z + worldToScreen.m[0][3];
		screen.y = worldToScreen.m[1][0] * point.x + worldToScreen.m[1][1] * point.y + worldToScreen.m[1][2] * point.z + worldToScreen.m[1][3];
		//screen.z = worldToScreen.m[2][0] * point.x + worldToScreen.m[2][1] * point.y + worldToScreen.m[2][2] * point.z + worldToScreen.m[2][3];
		float w = worldToScreen.m[3][0] * point.x + worldToScreen.m[3][1] * point.y + worldToScreen.m[3][2] * point.z + worldToScreen.m[3][3];

		screen.x /= w;
		screen.y /= w;
		//screen.z /= w;

		if (w < 0.001f)
			return false;
		else
		{
			float fScreenX = width / 2;
			float fScreenY = height / 2;

			fScreenX += 0.5f * screen.x * width + 0.5f;
			fScreenY -= 0.5f * screen.y * height + 0.5f;

			screen.x = fScreenX;
			screen.y = fScreenY;

			return true;
		}
	}

	void static ITranslate(Vector& in, matrix3x4_t& boneMat, Vector& out)
	{
		out.x = in.x - boneMat.m[0][3];
		out.y = in.y - boneMat.m[1][3];
		out.z = in.z - boneMat.m[2][3];
	}

	void static IRotate(Vector& in, matrix3x4_t& boneMat, Vector& out)
	{
		out.x = boneMat.m[0][0] * in.x + boneMat.m[1][0] * in.y + boneMat.m[2][0] * in.z;
		out.y = boneMat.m[0][1] * in.x + boneMat.m[1][1] * in.y + boneMat.m[2][1] * in.z;
		out.z = boneMat.m[0][2] * in.x + boneMat.m[1][2] * in.y + boneMat.m[2][2] * in.z;
	}

	void static ITransform(Vector& in, matrix3x4_t& boneMat, Vector& out)
	{
		Vector temp;
		temp.x = in.x - boneMat.m[0][3];
		temp.y = in.y - boneMat.m[1][3];
		temp.z = in.z - boneMat.m[2][3];

		out.x = boneMat.m[0][0] * temp.x + boneMat.m[1][0] * temp.y + boneMat.m[2][0] * temp.z;
		out.y = boneMat.m[0][1] * temp.x + boneMat.m[1][1] * temp.y + boneMat.m[2][1] * temp.z;
		out.z = boneMat.m[0][2] * temp.x + boneMat.m[1][2] * temp.y + boneMat.m[2][2] * temp.z;
	}

	Vector static MatMul(Vector& in, matrix3x4_t& boneMat)
	{
		Vector out;
		out.x = boneMat.m[0][0] * in.x + boneMat.m[0][1] * in.y + boneMat.m[0][2] * in.z + boneMat.m[0][3];
		out.y = boneMat.m[1][0] * in.x + boneMat.m[1][1] * in.y + boneMat.m[1][2] * in.z + boneMat.m[1][3];
		out.z = boneMat.m[2][0] * in.x + boneMat.m[2][1] * in.y + boneMat.m[2][2] * in.z + boneMat.m[2][3];

		return out;
	}

	Vector static MatrixAngles(const matrix3x4_t& matrix)
	{
		float forward[3];
		float left[3];
		float up[3];

		//
		// Extract the basis vectors from the matrix. Since we only need the Z
		// component of the up vector, we don't get X and Y.
		//
		forward[0] = matrix.m[0][0];
		forward[1] = matrix.m[1][0];
		forward[2] = matrix.m[2][0];
		left[0] = matrix.m[0][1];
		left[1] = matrix.m[1][1];
		left[2] = matrix.m[2][1];
		up[2] = matrix.m[2][2];

		Vector angles;
		float xyDist = sqrtf(forward[0] * forward[0] + forward[1] * forward[1]);

		// enough here to get angles?
		if (xyDist > 0.001f)
		{
			// (yaw)	y = ATAN( forward.y, forward.x );		-- in our space, forward is the X axis
			angles.y = D3DXToDegree(atan2f(forward[1], forward[0]));

			// (pitch)	x = ATAN( -forward.z, sqrt(forward.x*forward.x+forward.y*forward.y) );
			angles.x = D3DXToDegree(atan2f(-forward[2], xyDist));

			// (roll)	z = ATAN( left.z, up.z );
			angles.z = D3DXToDegree(atan2f(left[2], up[2]));
		}
		else	// forward is mostly Z, gimbal lock-
		{
			// (yaw)	y = ATAN( -left.x, left.y );			-- forward is mostly z, so use right for yaw
			angles.y = D3DXToDegree(atan2f(-left[0], left[1]));

			// (pitch)	x = ATAN( -forward.z, sqrt(forward.x*forward.x+forward.y*forward.y) );
			angles.x = D3DXToDegree(atan2f(-forward[2], xyDist));

			// Assume no roll in this case as one degree of freedom has been lost (i.e. yaw == roll)
			angles.z = 0;
		}

		return angles;
	}

	matrix3x4_t static AngleMatrix(Vector angles, Vector position)
	{
		float sy, cy, sp, cp, sr, cr;
		matrix3x4_t matrix;

		//pitch
		sp = sin(D3DXToRadian(angles.x));
		cp = cos(D3DXToRadian(angles.x));

		//yaw
		sy = sin(D3DXToRadian(angles.y));
		cy = cos(D3DXToRadian(angles.y));

		//roll
		sr = sin(D3DXToRadian(angles.z));
		cr = cos(D3DXToRadian(angles.z));

		// matrix = (YAW * PITCH) * ROLL
		matrix.m[0][0] = cp*cy;
		matrix.m[1][0] = cp*sy;
		matrix.m[2][0] = -sp;

		float crcy = cr*cy;
		float crsy = cr*sy;
		float srcy = sr*cy;
		float srsy = sr*sy;

		matrix.m[0][1] = sp*srcy - crsy;
		matrix.m[1][1] = sp*srsy + crcy;
		matrix.m[2][1] = sr*cp;

		matrix.m[0][2] = (sp*crcy + srsy);
		matrix.m[1][2] = (sp*crsy - srcy);
		matrix.m[2][2] = cr*cp;

		matrix.m[0][3] = position.x;
		matrix.m[1][3] = position.y;
		matrix.m[2][3] = position.z;

		return matrix;
	}
};