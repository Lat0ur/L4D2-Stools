#pragma once
#include "windows.h"

class Color
{
public:
	// constructors
	Color()
	{
		*((int *)this) = 0;
	}
	Color(int r, int g, int b)
	{
		SetColor(r, g, b, 0);
	}
	Color(int r, int g, int b, int a)
	{
		SetColor(r, g, b, a);
	}

	// set the color
	// r - red component (0-255)
	// g - green component (0-255)
	// b - blue component (0-255)
	// a - alpha component, controls transparency (0 - transparent, 255 - opaque);
	void SetColor(int r, int g, int b, int a = 0)
	{
		_color[0] = (unsigned char)r;
		_color[1] = (unsigned char)g;
		_color[2] = (unsigned char)b;
		_color[3] = (unsigned char)a;
	}

	void GetColor(int &r, int &g, int &b, int &a) const
	{
		r = _color[0];
		g = _color[1];
		b = _color[2];
		a = _color[3];
	}

	void SetRawColor(int color32)
	{
		*((int *)this) = color32;
	}

	int GetRawColor() const
	{
		return *((int *)this);
	}

	inline int r() const { return _color[0]; }
	inline int g() const { return _color[1]; }
	inline int b() const { return _color[2]; }
	inline int a() const { return _color[3]; }

	unsigned char &operator[](int index)
	{
		return _color[index];
	}

	const unsigned char &operator[](int index) const
	{
		return _color[index];
	}

	bool operator == (const Color &rhs) const
	{
		return (*((int *)this) == *((int *)&rhs));
	}

	bool operator != (const Color &rhs) const
	{
		return !(operator==(rhs));
	}

	Color &operator=(const Color &rhs)
	{
		SetRawColor(rhs.GetRawColor());
		return *this;
	}

private:
	unsigned char _color[4];
};


void L4D2_Msg(const char* message) {
	typedef void(*addr)(const char* pMsg, ...);
	addr msg;
	msg = (addr)GetProcAddress(GetModuleHandleA("tier0.dll"), "Msg");
	msg(message);
}
void L4D2_Warning(const char* message) {
	typedef void(*addr)(const char* pMsg, ...);
	addr warning;
	warning = (addr)GetProcAddress(GetModuleHandleA("tier0.dll"), "Warning");
	warning(message);
}

void L4D2_ConColorMsg(const Color& color, const char* message, ...) {
	typedef void(*addr)(const Color& clr, const char* pMsg, ...);
	addr concolor;
	concolor = (addr)GetProcAddress(GetModuleHandleA("tier0.dll"), "?ConColorMsg@@YAXABVColor@@PBDZZ");
	concolor(color, message);
}


class OverlayText_t;
class Vector;
struct Color;

class IVDebugOverlay
{
public:
	virtual void AddEntityTextOverlay(int ent_index, int line_offset, float duration, int r, int g, int b, int a, const char *format, ...) = 0;
	virtual void AddBoxOverlay(const Vector& origin, const Vector& mins, const Vector& max, QAngle const& orientation, int r, int g, int b, int a, float duration) = 0;
	virtual void AddTriangleOverlay(const Vector& p1, const Vector& p2, const Vector& p3, int r, int g, int b, int a, bool noDepthTest, float duration) = 0;
	virtual void AddLineOverlay(const Vector& origin, const Vector& dest, int r, int g, int b, bool noDepthTest, float duration) = 0;
	virtual void AddTextOverlay(const Vector& origin, float duration, const char *format, ...) = 0;
	virtual void AddTextOverlay(const Vector& origin, int line_offset, float duration, const char *format, ...) = 0;
	virtual void AddScreenTextOverlay(float flXPos, float flYPos, float flDuration, int r, int g, int b, int a, const char *text) = 0;
	virtual void AddSweptBoxOverlay(const Vector& start, const Vector& end, const Vector& mins, const Vector& max, const QAngle & angles, int r, int g, int b, int a, float flDuration) = 0;
	virtual void AddGridOverlay(const Vector& origin) = 0;
	virtual int ScreenPosition(const Vector& point, Vector& screen) = 0;
	virtual int ScreenPosition(float flXPos, float flYPos, Vector& screen) = 0;

	virtual OverlayText_t *GetFirst(void) = 0;
	virtual OverlayText_t *GetNext(OverlayText_t *current) = 0;
	virtual void ClearDeadOverlays(void) = 0;
	virtual void ClearAllOverlays() = 0;

	virtual void AddTextOverlayRGB(const Vector& origin, int line_offset, float duration, float r, float g, float b, float alpha, const char *format, ...) = 0;
	virtual void AddTextOverlayRGB(const Vector& origin, int line_offset, float duration, int r, int g, int b, int a, const char *format, ...) = 0;

	virtual void AddLineOverlayAlpha(const Vector& origin, const Vector& dest, int r, int g, int b, int a, bool noDepthTest, float duration) = 0;
	virtual void AddBoxOverlay2(const Vector& origin, const Vector& mins, const Vector& max, QAngle const& orientation, const Color& faceColor, const Color& edgeColor, float duration) = 0;

private:
	inline void AddTextOverlay(const Vector& origin, int line_offset, float duration, int r, int g, int b, int a, const char *format, ...) {} /* catch improper use of bad interface. Needed because '0' duration can be resolved by compiler to NULL format string (i.e., compiles but calls wrong function) */
};


class IClientEntity;

class IClientNetworkable;
class CBaseHandle;
class IClientUnknown;

class IClientEntityList
{
public:
	// Get IClientNetworkable interface for specified entity
	virtual IClientNetworkable*	GetClientNetworkable(int entnum) = 0;
	virtual IClientNetworkable*	GetClientNetworkableFromHandle(CBaseHandle hEnt) = 0;
	virtual IClientUnknown*		GetClientUnknownFromHandle(CBaseHandle hEnt) = 0;

	// NOTE: This function is only a convenience wrapper.
	// It returns GetClientNetworkable( entnum )->GetIClientEntity().
	virtual IClientEntity*		GetClientEntity(int entnum) = 0;
	virtual IClientEntity*		GetClientEntityFromHandle(CBaseHandle hEnt) = 0;

	// Returns number of entities currently in use
	virtual int					NumberOfEntities(bool bIncludeNonNetworkable) = 0;

	// Returns highest index actually used
	virtual int					GetHighestEntityIndex(void) = 0;

	// Sizes entity list to specified size
	virtual void				SetMaxEntities(int maxents) = 0;
	virtual int					GetMaxEntities() = 0;
};