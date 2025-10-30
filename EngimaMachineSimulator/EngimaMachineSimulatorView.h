// EngimaMachineSimulatorView.h : interface of the CEngimaMachineSimulatorView class
//

#pragma once

#include <array>

class CEngimaMachineSimulatorView : public CView
{
protected: // create from serialization only
	CEngimaMachineSimulatorView() noexcept;
	DECLARE_DYNCREATE(CEngimaMachineSimulatorView)

// Attributes
public:
	CEngimaMachineSimulatorDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CEngimaMachineSimulatorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnEditPlainChanged();
	afx_msg void OnConfigChanged();
	afx_msg void OnComboChanged(UINT nID);
	afx_msg void OnResetClicked();
	afx_msg void OnRandomizeClicked();
	DECLARE_MESSAGE_MAP()

public:
	// Simple IDs for runtime-created controls
	enum : UINT {
		IDC_CB_REFLECTOR =2001,
		IDC_CB_L_ROTOR,
		IDC_CB_M_ROTOR,
		IDC_CB_R_ROTOR,
		IDC_CB_L_POS,
		IDC_CB_M_POS,
		IDC_CB_R_POS,
		IDC_CB_L_RING,
		IDC_CB_M_RING,
		IDC_CB_R_RING,
		IDC_EDIT_PLUGBOARD,
		IDC_EDIT_PLAIN,
		IDC_EDIT_CIPHER,
		IDC_BTN_RESET,
		IDC_BTN_RANDOM
	};

private:
	// UI controls
	CStatic m_lblReflector, m_lblRotors, m_lblPos, m_lblRing, m_lblPlug, m_lblPlain, m_lblCipher;
	CComboBox m_cbReflector, m_cbLeftRotor, m_cbMidRotor, m_cbRightRotor;
	CComboBox m_cbLeftPos, m_cbMidPos, m_cbRightPos;
	CComboBox m_cbLeftRing, m_cbMidRing, m_cbRightRing;
	CEdit m_edPlugboard, m_edPlain, m_edCipher;
	CButton m_btnReset, m_btnRandom;

	void CreateControls();
	void LayoutControls();
	void PopulateCombos();
	void UpdateCiphertext();
	void ResetSettings();
	void RandomizeSettings();
};

#ifndef _DEBUG  // debug version in EngimaMachineSimulatorView.cpp
inline CEngimaMachineSimulatorDoc* CEngimaMachineSimulatorView::GetDocument() const
   { return reinterpret_cast<CEngimaMachineSimulatorDoc*>(m_pDocument); }
#endif

