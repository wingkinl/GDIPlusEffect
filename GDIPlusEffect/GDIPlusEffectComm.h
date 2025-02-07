#pragma once

class GDIPlusEffect
{
public:
	GDIPlusEffect();
	virtual ~GDIPlusEffect();
public:
	virtual Gdiplus::Effect* GetEffect() = 0;

	virtual CMFCPropertyGridProperty* CreateProps() const = 0;

	virtual void OnUpdateProps(CMFCPropertyGridProperty* pPropsGroup
		, CMFCPropertyGridProperty* pProps) = 0;

	enum class Type {
		Blur,
		BrightnessContrast,
		ColorBalance,
		ColorCurve,
		ColorLUT,
		ColorMatrixEffect,
		HueSaturationLightness,
		Levels,
		RedEyeCorrection,
		Sharpen,
		Tint,

		Total,
	};

	virtual Type GetType() const = 0;

	static GDIPlusEffect* CreateEffect(Type nType);

	static LPCTSTR GetTypeName(Type nType);

	void AttachProps(CMFCPropertyGridProperty* pProp)
	{
		m_pProps = pProp;
	}

	void Enable(bool bEnable = true) {
		m_bEnable = bEnable;
	}

	bool IsEnabled() const {
		return m_bEnable;
	}
protected:
	CMFCPropertyGridProperty* m_pProps = nullptr;
	bool m_bEnable = true;
};


