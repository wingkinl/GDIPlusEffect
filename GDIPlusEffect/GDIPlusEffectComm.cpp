#include "pch.h"
#include "framework.h"
#include "GDIPlusEffectComm.h"
#include "MainFrm.h"
#include "GDIPlusEffectView.h"

class GDIPlusEffectProp : public CMFCPropertyGridProperty
{
public:
	using CMFCPropertyGridProperty::CMFCPropertyGridProperty;

	~GDIPlusEffectProp()
	{
		printf("");
	}
public:
	void AttachEffect(const GDIPlusEffect* pEffect)
	{
		m_pEffect = (GDIPlusEffect*)pEffect;
	}

	BOOL OnUpdateValue() override
	{
		auto bRet = __super::OnUpdateValue();
		if (bRet && m_pEffect)
		{
			auto pGroup = GetParent();
			if (pGroup)
			{
				m_pEffect->OnUpdateProps(pGroup, this);
				auto pView = CGDIPlusEffectView::GetActiveView();
				if (pView)
					pView->Invalidate();
			}
		}
		return bRet;
	}

	void OnDeleteProp()
	{
		CMFCPropertyGridProperty* pProp = this;
		m_pWndList->DeleteProperty(pProp, FALSE);
	}
protected:
	GDIPlusEffect* m_pEffect = nullptr;
};

GDIPlusEffect::GDIPlusEffect()
{

}

GDIPlusEffect::~GDIPlusEffect()
{
	if (m_pProps)
	{
		auto pProp = (GDIPlusEffectProp*)m_pProps;
		pProp->OnDeleteProp();
	}
}

class GDIPlusEffectBlur : public GDIPlusEffect
{
public:
	GDIPlusEffectBlur()
	{
		m_effect.SetParameters(&m_params);
	}

	Gdiplus::Effect* GetEffect() override
	{
		return &m_effect;
	}

	CMFCPropertyGridProperty* CreateProps() const override
	{
		auto pGroup = new GDIPlusEffectProp(GetTypeName(GetType()));
		auto pProp = new GDIPlusEffectProp(_T("radius"), (_variant_t)0,
			_T("Real number that specifies the blur radius (the radius of the Gaussian convolution kernel) in pixels. The radius must be in the range 0 through 255. As the radius increases, the resulting bitmap becomes more blurry."));
		pProp->EnableSpinControl(TRUE, 0, 255);
		pProp->AttachEffect(this);
		pGroup->AddSubItem(pProp);

		pProp = new GDIPlusEffectProp(_T("expandEdge"), (_variant_t)false,
			_T("Specifies whether the bitmap expands by an amount equal to the blur radius. If TRUE, the bitmap expands by an amount equal to the radius so that it can have soft edges. If FALSE, the bitmap remains the same size and the soft edges are clipped."));
		pProp->AttachEffect(this);
		pGroup->AddSubItem(pProp);

		return pGroup;
	}

	Type GetType() const override
	{
		return Type::Blur;
	}

	void OnUpdateProps(CMFCPropertyGridProperty* pGroup, CMFCPropertyGridProperty* pProp) override
	{
		auto pSubProp = pGroup->GetSubItem(0);
		auto val = pSubProp->GetValue();
		m_params.radius = (float)val.intVal;

		pSubProp = pGroup->GetSubItem(1);
		val = pSubProp->GetValue();
		m_params.expandEdge = (bool)val.boolVal;

		m_effect.SetParameters(&m_params);
	}
protected:
	Gdiplus::Blur		m_effect;
	Gdiplus::BlurParams m_params = { 0 };
};

class GDIPlusEffectBrightnessContrast : public GDIPlusEffect
{
public:
	GDIPlusEffectBrightnessContrast()
	{
		m_effect.SetParameters(&m_params);
	}

	Gdiplus::Effect* GetEffect() override
	{
		return &m_effect;
	}

	CMFCPropertyGridProperty* CreateProps() const override
	{
		auto pGroup = new GDIPlusEffectProp(GetTypeName(GetType()));
		auto pProp = new GDIPlusEffectProp(_T("brightnessLevel"), (_variant_t)0
			, _T("Specifies the brightness level. If the value is 0, the brightness remains the same. As the value moves from 0 to 255, the brightness of the image increases. As the value moves from 0 to -255, the brightness of the image decreases."));
		pProp->EnableSpinControl(TRUE, -255, 255);
		pProp->AttachEffect(this);
		pGroup->AddSubItem(pProp);

		pProp = new GDIPlusEffectProp(_T("contrastLevel"), (_variant_t)0
			, _T("Specifies the contrast level. If the value is 0, the contrast remains the same. As the value moves from 0 to 100, the contrast of the image increases. As the value moves from 0 to -100, the contrast of the image decreases."));
		pProp->EnableSpinControl(TRUE, -100, 100);
		pProp->AttachEffect(this);
		pGroup->AddSubItem(pProp);

		return pGroup;
	}

	Type GetType() const override
	{
		return Type::BrightnessContrast;
	}

	void OnUpdateProps(CMFCPropertyGridProperty* pGroup, CMFCPropertyGridProperty* pProp) override
	{
		auto pSubProp = pGroup->GetSubItem(0);
		auto val = pSubProp->GetValue();
		m_params.brightnessLevel = val.intVal;

		pSubProp = pGroup->GetSubItem(1);
		val = pSubProp->GetValue();
		m_params.contrastLevel = val.intVal;

		m_effect.SetParameters(&m_params);
	}
protected:
	Gdiplus::BrightnessContrast		m_effect;
	Gdiplus::BrightnessContrastParams m_params = { 0 };
};

class GDIPlusEffectColorBalance : public GDIPlusEffect
{
public:
	GDIPlusEffectColorBalance()
	{
		m_effect.SetParameters(&m_params);
	}

	Gdiplus::Effect* GetEffect() override
	{
		return &m_effect;
	}

	CMFCPropertyGridProperty* CreateProps() const override
	{
		auto pGroup = new GDIPlusEffectProp(GetTypeName(GetType()));
		auto pProp = new GDIPlusEffectProp(_T("cyanRed"), (_variant_t)0
			, _T("specifies a change in the amount of red in the image. If the value is 0, there is no change. As the value moves from 0 to 100, the amount of red in the image increases and the amount of cyan decreases. As the value moves from 0 to -100, the amount of red in the image decreases and the amount of cyan increases."));
		pProp->EnableSpinControl(TRUE, -100, 100);
		pProp->AttachEffect(this);
		pGroup->AddSubItem(pProp);

		pProp = new GDIPlusEffectProp(_T("magentaGreen"), (_variant_t)0
			, _T("specifies a change in the amount of green in the image. If the value is 0, there is no change. As the value moves from 0 to 100, the amount of green in the image increases and the amount of magenta decreases. As the value moves from 0 to -100, the amount of green in the image decreases and the amount of magenta increases."));
		pProp->EnableSpinControl(TRUE, -100, 100);
		pProp->AttachEffect(this);
		pGroup->AddSubItem(pProp);

		pProp = new GDIPlusEffectProp(_T("yellowBlue"), (_variant_t)0
			, _T("specifies a change in the amount of blue in the image. If the value is 0, there is no change. As the value moves from 0 to 100, the amount of blue in the image increases and the amount of yellow decreases. As the value moves from 0 to -100, the amount of blue in the image decreases and the amount of yellow increases."));
		pProp->EnableSpinControl(TRUE, -100, 100);
		pProp->AttachEffect(this);
		pGroup->AddSubItem(pProp);

		return pGroup;
	}

	Type GetType() const override
	{
		return Type::ColorBalance;
	}

	void OnUpdateProps(CMFCPropertyGridProperty* pGroup, CMFCPropertyGridProperty* pProp) override
	{
		auto pSubProp = pGroup->GetSubItem(0);
		auto val = pSubProp->GetValue();
		m_params.cyanRed = val.intVal;

		pSubProp = pGroup->GetSubItem(1);
		val = pSubProp->GetValue();
		m_params.magentaGreen = val.intVal;

		pSubProp = pGroup->GetSubItem(2);
		val = pSubProp->GetValue();
		m_params.yellowBlue = val.intVal;

		m_effect.SetParameters(&m_params);
	}
protected:
	Gdiplus::ColorBalance		m_effect;
	Gdiplus::ColorBalanceParams m_params = { 0 };
};

class GDIPlusEffectHueSaturationLightness : public GDIPlusEffect
{
public:
	GDIPlusEffectHueSaturationLightness()
	{
		m_effect.SetParameters(&m_params);
	}

	Gdiplus::Effect* GetEffect() override
	{
		return &m_effect;
	}

	CMFCPropertyGridProperty* CreateProps() const override
	{
		auto pGroup = new GDIPlusEffectProp(GetTypeName(GetType()));
		auto pProp = new GDIPlusEffectProp(_T("hueLevel"), (_variant_t)0
			, _T("specifies the change in hue. A value of 0 specifies no change. Positive values specify counterclockwise rotation on the color wheel. Negative values specify clockwise rotation on the color wheel."));
		pProp->EnableSpinControl(TRUE, -180, 180);
		pProp->AttachEffect(this);
		pGroup->AddSubItem(pProp);

		pProp = new GDIPlusEffectProp(_T("saturationLevel"), (_variant_t)0
			, _T("specifies the change in saturation. A value of 0 specifies no change. Positive values specify increased saturation and negative values specify decreased saturation."));
		pProp->EnableSpinControl(TRUE, -100, 100);
		pProp->AttachEffect(this);
		pGroup->AddSubItem(pProp);

		pProp = new GDIPlusEffectProp(_T("lightnessLevel"), (_variant_t)0
			, _T("specifies the change in lightness. A value of 0 specifies no change. Positive values specify increased lightness and negative values specify decreased lightness."));
		pProp->EnableSpinControl(TRUE, -100, 100);
		pProp->AttachEffect(this);
		pGroup->AddSubItem(pProp);

		return pGroup;
	}

	Type GetType() const override
	{
		return Type::HueSaturationLightness;
	}

	void OnUpdateProps(CMFCPropertyGridProperty* pGroup, CMFCPropertyGridProperty* pProp) override
	{
		auto pSubProp = pGroup->GetSubItem(0);
		auto val = pSubProp->GetValue();
		m_params.hueLevel = val.intVal;

		pSubProp = pGroup->GetSubItem(1);
		val = pSubProp->GetValue();
		m_params.saturationLevel = val.intVal;

		pSubProp = pGroup->GetSubItem(2);
		val = pSubProp->GetValue();
		m_params.lightnessLevel = val.intVal;

		m_effect.SetParameters(&m_params);
	}
protected:
	Gdiplus::HueSaturationLightness		m_effect;
	Gdiplus::HueSaturationLightnessParams  m_params = { 0 };
};

class GDIPlusEffectLevels : public GDIPlusEffect
{
public:
	GDIPlusEffectLevels()
	{
		m_effect.SetParameters(&m_params);
	}

	Gdiplus::Effect* GetEffect() override
	{
		return &m_effect;
	}

	CMFCPropertyGridProperty* CreateProps() const override
	{
		auto pGroup = new GDIPlusEffectProp(GetTypeName(GetType()));
		auto pProp = new GDIPlusEffectProp(_T("highlight"), (_variant_t)100
			, _T("specifies which pixels should be lightened. You can use this adjustment to lighten pixels that are already lighter than a certain threshold. Setting highlight to 100 specifies no change. Setting highlight to t specifies that a color channel value is increased if it is already greater than t percent of full intensity. For example, setting highlight to 90 specifies that all color channel values greater than 90 percent of full intensity are increased."));
		pProp->EnableSpinControl(TRUE, 0, 100);
		pProp->AttachEffect(this);
		pGroup->AddSubItem(pProp);

		pProp = new GDIPlusEffectProp(_T("midtone"), (_variant_t)0
			, _T("specifies how much to lighten or darken an image. Color channel values in the middle of the intensity range are altered more than color channel values near the minimum or maximum intensity. You can use this adjustment to lighten (or darken) an image without loosing the contrast between the darkest and lightest portions of the image. A value of 0 specifies no change. Positive values specify that the midtones are made lighter, and negative values specify that the midtones are made darker."));
		pProp->EnableSpinControl(TRUE, -100, 100);
		pProp->AttachEffect(this);
		pGroup->AddSubItem(pProp);

		pProp = new GDIPlusEffectProp(_T("shadow"), (_variant_t)0
			, _T("specifies which pixels should be darkened. You can use this adjustment to darken pixels that are already darker than a certain threshold. Setting shadow to 0 specifies no change. Setting shadow to t specifies that a color channel value is decreased if it is already less than t percent of full intensity. For example, setting shadow to 10 specifies that all color channel values less than 10 percent of full intensity are decreased."));
		pProp->EnableSpinControl(TRUE, 0, 100);
		pProp->AttachEffect(this);
		pGroup->AddSubItem(pProp);

		return pGroup;
	}

	Type GetType() const override
	{
		return Type::Levels;
	}

	void OnUpdateProps(CMFCPropertyGridProperty* pGroup, CMFCPropertyGridProperty* pProp) override
	{
		auto pSubProp = pGroup->GetSubItem(0);
		auto val = pSubProp->GetValue();
		m_params.highlight = val.intVal;

		pSubProp = pGroup->GetSubItem(1);
		val = pSubProp->GetValue();
		m_params.midtone = val.intVal;

		pSubProp = pGroup->GetSubItem(2);
		val = pSubProp->GetValue();
		m_params.shadow = val.intVal;

		m_effect.SetParameters(&m_params);
	}
protected:
	Gdiplus::Levels		m_effect;
	Gdiplus::LevelsParams  m_params = { 100 };
};

class GDIPlusEffectTint : public GDIPlusEffect
{
public:
	GDIPlusEffectTint()
	{
		m_effect.SetParameters(&m_params);
	}

	Gdiplus::Effect* GetEffect() override
	{
		return &m_effect;
	}

	CMFCPropertyGridProperty* CreateProps() const override
	{
		auto pGroup = new GDIPlusEffectProp(GetTypeName(GetType()));
		auto pProp = new GDIPlusEffectProp(_T("hue"), (_variant_t)0
			, _T("specifies the hue to be strengthened or weakened. A value of 0 specifies blue. A positive value specifies a clockwise angle on the color wheel. For example, positive 60 specifies cyan and positive 120 specifies green. A negative value specifies a counter-clockwise angle on the color wheel. For example, negative 60 specifies magenta and negative 120 specifies red."));
		pProp->EnableSpinControl(TRUE, -180, 180);
		pProp->AttachEffect(this);
		pGroup->AddSubItem(pProp);

		pProp = new GDIPlusEffectProp(_T("amount"), (_variant_t)0
			, _T("specifies how much the hue (given by the hue parameter) is strengthened or weakened. A value of 0 specifies no change. Positive values specify that the hue is strengthened and negative values specify that the hue is weakened."));
		pProp->EnableSpinControl(TRUE, -100, 100);
		pProp->AttachEffect(this);
		pGroup->AddSubItem(pProp);

		return pGroup;
	}

	Type GetType() const override
	{
		return Type::Tint;
	}

	void OnUpdateProps(CMFCPropertyGridProperty* pGroup, CMFCPropertyGridProperty* pProp) override
	{
		auto pSubProp = pGroup->GetSubItem(0);
		auto val = pSubProp->GetValue();
		m_params.hue = val.intVal;

		pSubProp = pGroup->GetSubItem(1);
		val = pSubProp->GetValue();
		m_params.amount = val.intVal;

		m_effect.SetParameters(&m_params);
	}
protected:
	Gdiplus::Tint		m_effect;
	Gdiplus::TintParams  m_params = { 0 };
};

class GDIPlusEffectSharpen : public GDIPlusEffect
{
public:
	GDIPlusEffectSharpen()
	{
		m_effect.SetParameters(&m_params);
	}

	Gdiplus::Effect* GetEffect() override
	{
		return &m_effect;
	}

	CMFCPropertyGridProperty* CreateProps() const override
	{
		auto pGroup = new GDIPlusEffectProp(GetTypeName(GetType()));
		auto pProp = new GDIPlusEffectProp(_T("radius"), (_variant_t)0.0f
			, _T("Real number that specifies the sharpening radius (the radius of the convolution kernel) in pixels. The radius must be in the range 0 through 255. As the radius increases, more surrounding pixels are involved in calculating the new value of a given pixel."));
		//pProp->EnableSpinControl(TRUE, 0, 255);
		pProp->AttachEffect(this);
		pGroup->AddSubItem(pProp);

		pProp = new GDIPlusEffectProp(_T("amount"), (_variant_t)0.0f
			, _T("Real number in the range 0 through 100 that specifies the amount of sharpening to be applied. A value of 0 specifies no sharpening. As the value of amount increases, the sharpness increases."));
		//pProp->EnableSpinControl(TRUE, 0, 100);
		pProp->AttachEffect(this);
		pGroup->AddSubItem(pProp);

		return pGroup;
	}

	Type GetType() const override
	{
		return Type::Sharpen;
	}

	void OnUpdateProps(CMFCPropertyGridProperty* pGroup, CMFCPropertyGridProperty* pProp) override
	{
		auto pSubProp = pGroup->GetSubItem(0);
		auto val = pSubProp->GetValue();
		m_params.radius = val.fltVal;

		pSubProp = pGroup->GetSubItem(1);
		val = pSubProp->GetValue();
		m_params.amount = val.fltVal;

		m_effect.SetParameters(&m_params);
	}
protected:
	Gdiplus::Sharpen		m_effect;
	Gdiplus::SharpenParams  m_params = { 0 };
};

class GDIPlusEffectRedEyeCorrection : public GDIPlusEffect
{
public:
	GDIPlusEffectRedEyeCorrection()
	{
		m_effect.SetParameters(&m_params);
	}

	Gdiplus::Effect* GetEffect() override
	{
		return &m_effect;
	}

	CMFCPropertyGridProperty* CreateProps() const override
	{
		auto pGroup = new GDIPlusEffectProp(GetTypeName(GetType()));

		return pGroup;
	}

	Type GetType() const override
	{
		return Type::RedEyeCorrection;
	}

	void OnUpdateProps(CMFCPropertyGridProperty* pGroup, CMFCPropertyGridProperty* pProp) override
	{
		m_params.numberOfAreas = 1;

		CRect rect;
		auto pView = CGDIPlusEffectView::GetActiveView();
		if (pView)
		{
			pView->GetClientRect(&rect);
			m_params.areas = &rect;
		}

		m_effect.SetParameters(&m_params);
	}
protected:
	Gdiplus::RedEyeCorrection		m_effect;
	Gdiplus::RedEyeCorrectionParams  m_params = { 0 };
};

static int _GetPropOptionValue(CMFCPropertyGridProperty& prop)
{
	auto val = prop.GetValue();
	CString strVal = val.bstrVal;
	int nCount = prop.GetOptionCount();
	for (int ii = 0; ii < nCount; ++ii)
	{
		if (strVal == prop.GetOption(ii))
		{
			return ii;
		}
	}
	return -1;
}

class GDIPlusColorCurveEffectAdjustmentsProp : public GDIPlusEffectProp
{
public:
	using GDIPlusEffectProp::GDIPlusEffectProp;

public:
	BOOL OnUpdateValue() override
	{
		auto bRet = __super::OnUpdateValue();
		if (bRet)
		{
			auto val = GetValue();
			int nVal = _GetPropOptionValue(*this);

			auto nAdjust = (Gdiplus::CurveAdjustments)nVal;
			int nMin = -255;
			int nMax = 255;
			switch (nAdjust)
			{
			case Gdiplus::AdjustExposure:
				nMin = -255;
				nMax = 255;
				break;
			case Gdiplus::AdjustDensity:
				nMin = -255;
				nMax = 255;
				break;
			case Gdiplus::AdjustContrast:
				nMin = -100;
				nMax = 100;
				break;
			case Gdiplus::AdjustHighlight:
				nMin = -100;
				nMax = 100;
				break;
			case Gdiplus::AdjustShadow:
				nMin = -100;
				nMax = 100;
				break;
			case Gdiplus::AdjustMidtone:
				nMin = -100;
				nMax = 100;
				break;
			case Gdiplus::AdjustWhiteSaturation:
				nMin = 0;
				nMax = 255;
				break;
			case Gdiplus::AdjustBlackSaturation:
				nMin = 0;
				nMax = 255;
				break;
			}
			auto pGroup = GetParent();
			auto pAdjustValProp = pGroup->GetSubItem(2);
			pAdjustValProp->EnableSpinControl(TRUE, nMin, nMax);
		}
		return bRet;
	}
};

class GDIPlusEffectColorCurve : public GDIPlusEffect
{
public:
	GDIPlusEffectColorCurve()
	{
		m_effect.SetParameters(&m_params);
	}

	Gdiplus::Effect* GetEffect() override
	{
		return &m_effect;
	}

	CMFCPropertyGridProperty* CreateProps() const override
	{
		auto pGroup = new GDIPlusEffectProp(GetTypeName(GetType()));
		
		GDIPlusEffectProp* pProp = new GDIPlusColorCurveEffectAdjustmentsProp(_T("adjustment"), _T("Exposure")
			, _T("Element of the CurveAdjustments enumeration that specifies the adjustment to be applied."));

		pProp->AddOption(_T("Exposure"));
		pProp->AddOption(_T("Density"));
		pProp->AddOption(_T("Contrast"));
		pProp->AddOption(_T("Highlight"));
		pProp->AddOption(_T("Shadow"));
		pProp->AddOption(_T("Midtone"));
		pProp->AddOption(_T("WhiteSaturation"));
		pProp->AddOption(_T("BlackSaturation"));

		pProp->AllowEdit(FALSE);

		pProp->AttachEffect(this);
		pGroup->AddSubItem(pProp);

		pProp = new GDIPlusEffectProp(_T("channel"), _T("All")
			, _T("Element of the CurveChannel enumeration that specifies the color channel to which the adjustment applies."));
		pProp->AttachEffect(this);

		pProp->AddOption(_T("All"));
		pProp->AddOption(_T("Red"));
		pProp->AddOption(_T("Green"));
		pProp->AddOption(_T("Blue"));

		pGroup->AddSubItem(pProp);

		pProp = new GDIPlusEffectProp(_T("adjustValue"), (_variant_t)0
			, _T("Integer that specifies the intensity of the adjustment. The range of acceptable values depends on which adjustment is being applied."));
		pProp->AttachEffect(this);

		pProp->EnableSpinControl(TRUE, -255, 255);
		pGroup->AddSubItem(pProp);

		return pGroup;
	}

	Type GetType() const override
	{
		return Type::ColorCurve;
	}

	void OnUpdateProps(CMFCPropertyGridProperty* pGroup, CMFCPropertyGridProperty* pProp) override
	{
		auto pSubProp = pGroup->GetSubItem(0);
		int nAdjust = _GetPropOptionValue(*pSubProp);
		m_params.adjustment = (Gdiplus::CurveAdjustments)nAdjust;

		pSubProp = pGroup->GetSubItem(1);
		auto val = pSubProp->GetValue();
		m_params.channel = (Gdiplus::CurveChannel)val.intVal;

		pSubProp = pGroup->GetSubItem(2);
		val = pSubProp->GetValue();
		m_params.adjustValue = val.intVal;

		m_effect.SetParameters(&m_params);
	}
protected:
	Gdiplus::ColorCurve		m_effect;
	Gdiplus::ColorCurveParams  m_params = { 0 };
};

class GDIPlusEffectColorMatrix : public GDIPlusEffect
{
public:
	GDIPlusEffectColorMatrix()
	{
		for (int ii = 0; ii < 5; ++ii)
		{
			for (int jj = 0; jj < 5; ++jj)
			{
				if (ii == jj)
				{
					m_params.m[ii][jj] = 1.0f;
					break;
				}
			}
		}
		m_effect.SetParameters(&m_params);
	}

	Gdiplus::Effect* GetEffect() override
	{
		return &m_effect;
	}

	CMFCPropertyGridProperty* CreateProps() const override
	{
		auto pGroup = new GDIPlusEffectProp(GetTypeName(GetType()));

		for (int ii = 0; ii < 5; ++ii)
		{
			for (int jj = 0; jj < 5; ++jj)
			{
				CString str;
				str.Format(_T("m[%d][%d]"), ii, jj);
				float fDefault = m_params.m[ii][jj];
				auto pProp = new GDIPlusEffectProp(str, (_variant_t)fDefault
					, _T(""));
				pProp->AttachEffect(this);
				pGroup->AddSubItem(pProp);
			}
		}

		return pGroup;
	}

	Type GetType() const override
	{
		return Type::ColorMatrixEffect;
	}

	void OnUpdateProps(CMFCPropertyGridProperty* pGroup, CMFCPropertyGridProperty* pProp) override
	{
		for (int ii = 0; ii < 5; ++ii)
		{
			for (int jj = 0; jj < 5; ++jj)
			{
				auto pSubProp = pGroup->GetSubItem(ii*5+jj);
				auto val = pSubProp->GetValue();
				m_params.m[ii][jj] = val.fltVal;
			}
		}

		m_effect.SetParameters(&m_params);
	}
protected:
	Gdiplus::ColorMatrixEffect		m_effect;
	Gdiplus::ColorMatrix  m_params = { 0 };
};

class GDIPlusEffectColorLUT : public GDIPlusEffect
{
public:
	GDIPlusEffectColorLUT()
	{
		for (int ii = 0; ii < _countof(m_params.lutB); ++ii)
			m_params.lutB[ii] = ii;
		memcpy(m_params.lutG, m_params.lutB, sizeof(m_params.lutB));
		memcpy(m_params.lutR, m_params.lutB, sizeof(m_params.lutB));
		memcpy(m_params.lutA, m_params.lutB, sizeof(m_params.lutB));
		m_effect.SetParameters(&m_params);
	}

	Gdiplus::Effect* GetEffect() override
	{
		return &m_effect;
	}

	CMFCPropertyGridProperty* CreateProps() const override
	{
		auto pGroup = new GDIPlusEffectProp(GetTypeName(GetType()));

		const Gdiplus::ColorChannelLUT* arrColorChannels[] = {
			&m_params.lutB,
			&m_params.lutG,
			&m_params.lutR,
			&m_params.lutA,
		};
		const TCHAR arrChannelName[] = {
			_T('B'),_T('G'),_T('R'),_T('A')
		};

		for (int ii = 0; ii < _countof(arrColorChannels); ++ii)
		{
			for (int jj = 0; jj < _countof(m_params.lutB); ++jj)
			{
				CString str;
				str.Format(_T("lut%c[%d]"), arrChannelName[ii], jj);
				int nDefault = (*arrColorChannels[ii])[jj];
				auto pProp = new GDIPlusEffectProp(str, (_variant_t)nDefault
					, _T(""));
				pProp->AttachEffect(this);
				pProp->EnableSpinControl(TRUE, 0, 255);
				pGroup->AddSubItem(pProp);
			}
		}

		return pGroup;
	}

	Type GetType() const override
	{
		return Type::ColorLUT;
	}

	void OnUpdateProps(CMFCPropertyGridProperty* pGroup, CMFCPropertyGridProperty* pProp) override
	{
		Gdiplus::ColorChannelLUT* arrColorChannels[] = {
			&m_params.lutB,
			&m_params.lutG,
			&m_params.lutR,
			&m_params.lutA,
		};
		for (int ii = 0; ii < _countof(arrColorChannels); ++ii)
		{
			for (int jj = 0; jj < _countof(m_params.lutB); ++jj)
			{
				auto pSubProp = pGroup->GetSubItem(ii * _countof(m_params.lutB) + jj);
				auto val = pSubProp->GetValue();
				(*arrColorChannels[ii])[jj] = val.intVal;
			}
		}

		m_effect.SetParameters(&m_params);
	}
protected:
	Gdiplus::ColorLUT		m_effect;
	Gdiplus::ColorLUTParams  m_params = { 0 };
};

GDIPlusEffect* GDIPlusEffect::CreateEffect(Type nType)
{
	switch (nType)
	{
	case Type::Blur:
		return new GDIPlusEffectBlur;
	case Type::BrightnessContrast:
		return new GDIPlusEffectBrightnessContrast;
	case Type::ColorBalance:
		return new GDIPlusEffectColorBalance;
	case Type::ColorCurve:
		return new GDIPlusEffectColorCurve;
	case Type::ColorLUT:
		return new GDIPlusEffectColorLUT;
	case Type::ColorMatrixEffect:
		return new GDIPlusEffectColorMatrix;
	case Type::HueSaturationLightness:
		return new GDIPlusEffectHueSaturationLightness;
	case Type::Levels:
		return new GDIPlusEffectLevels;
	case Type::RedEyeCorrection:
		return new GDIPlusEffectRedEyeCorrection;
	case Type::Sharpen:
		return new GDIPlusEffectSharpen;
	case Type::Tint:
		return new GDIPlusEffectTint;
	default:
		break;
	}
	return nullptr;
}

LPCTSTR GDIPlusEffect::GetTypeName(Type nType)
{
	LPCTSTR vNames[] = {
		_T("Blur"),
		_T("BrightnessContrast"),
		_T("ColorBalance"),
		_T("ColorCurve"),
		_T("ColorLUT"),
		_T("ColorMatrixEffect"),
		_T("HueSaturationLightness"),
		_T("Levels"),
		_T("RedEyeCorrection"),
		_T("Sharpen"),
		_T("Tint")
	};
	return vNames[(int)nType];
}
