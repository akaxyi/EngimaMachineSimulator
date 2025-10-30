// EngimaMachineSimulatorView.cpp : implementation of the CEngimaMachineSimulatorView class
//

#include "pch.h"
#include "framework.h"
#ifndef SHARED_HANDLERS
#include "EngimaMachineSimulator.h"
#endif

#include "EngimaMachineSimulatorDoc.h"
#include "EngimaMachineSimulatorView.h"
#include "Enigma.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace EnigmaCore;

// CEngimaMachineSimulatorView

IMPLEMENT_DYNCREATE(CEngimaMachineSimulatorView, CView)

BEGIN_MESSAGE_MAP(CEngimaMachineSimulatorView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CEngimaMachineSimulatorView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()
	ON_CONTROL(EN_CHANGE, CEngimaMachineSimulatorView::IDC_EDIT_PLAIN, &CEngimaMachineSimulatorView::OnEditPlainChanged)
	ON_CONTROL(EN_CHANGE, CEngimaMachineSimulatorView::IDC_EDIT_PLUGBOARD, &CEngimaMachineSimulatorView::OnConfigChanged)
	ON_CONTROL_RANGE(CBN_SELCHANGE, CEngimaMachineSimulatorView::IDC_CB_REFLECTOR, CEngimaMachineSimulatorView::IDC_CB_R_RING, &CEngimaMachineSimulatorView::OnComboChanged)
	ON_BN_CLICKED(CEngimaMachineSimulatorView::IDC_BTN_RESET, &CEngimaMachineSimulatorView::OnResetClicked)
	ON_BN_CLICKED(CEngimaMachineSimulatorView::IDC_BTN_RANDOM, &CEngimaMachineSimulatorView::OnRandomizeClicked)
END_MESSAGE_MAP()

// CEngimaMachineSimulatorView construction/destruction

CEngimaMachineSimulatorView::CEngimaMachineSimulatorView() noexcept
{
}

CEngimaMachineSimulatorView::~CEngimaMachineSimulatorView()
{
}

BOOL CEngimaMachineSimulatorView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

void CEngimaMachineSimulatorView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	CreateControls();
	PopulateCombos();
	ResetSettings();
	LayoutControls();
}

// Layout helpers
void CEngimaMachineSimulatorView::CreateControls()
{
	CRect rc; GetClientRect(&rc);
	DWORD wsLbl = WS_CHILD | WS_VISIBLE;
	DWORD wsCb = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL;
	DWORD wsEd = WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN;

	m_lblReflector.Create(_T("Reflector"), wsLbl, CRect(0,0,0,0), this);
	m_cbReflector.Create(wsCb, CRect(0,0,0,0), this, IDC_CB_REFLECTOR);

	m_lblRotors.Create(_T("Rotors (L-M-R)"), wsLbl, CRect(0,0,0,0), this);
	m_cbLeftRotor.Create(wsCb, CRect(0,0,0,0), this, IDC_CB_L_ROTOR);
	m_cbMidRotor.Create(wsCb, CRect(0,0,0,0), this, IDC_CB_M_ROTOR);
	m_cbRightRotor.Create(wsCb, CRect(0,0,0,0), this, IDC_CB_R_ROTOR);

	m_lblPos.Create(_T("Positions"), wsLbl, CRect(0,0,0,0), this);
	m_cbLeftPos.Create(wsCb, CRect(0,0,0,0), this, IDC_CB_L_POS);
	m_cbMidPos.Create(wsCb, CRect(0,0,0,0), this, IDC_CB_M_POS);
	m_cbRightPos.Create(wsCb, CRect(0,0,0,0), this, IDC_CB_R_POS);

	m_lblRing.Create(_T("Rings"), wsLbl, CRect(0,0,0,0), this);
	m_cbLeftRing.Create(wsCb, CRect(0,0,0,0), this, IDC_CB_L_RING);
	m_cbMidRing.Create(wsCb, CRect(0,0,0,0), this, IDC_CB_M_RING);
	m_cbRightRing.Create(wsCb, CRect(0,0,0,0), this, IDC_CB_R_RING);

	m_lblPlug.Create(_T("Plugboard (pairs e.g. AB CD)"), wsLbl, CRect(0,0,0,0), this);
	m_edPlugboard.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, CRect(0,0,0,0), this, IDC_EDIT_PLUGBOARD);

	m_btnReset.Create(_T("Reset"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(0,0,0,0), this, IDC_BTN_RESET);
	m_btnRandom.Create(_T("Randomize"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(0,0,0,0), this, IDC_BTN_RANDOM);

	m_lblPlain.Create(_T("Plaintext"), wsLbl, CRect(0,0,0,0), this);
	m_edPlain.Create(wsEd, CRect(0,0,0,0), this, IDC_EDIT_PLAIN);

	m_lblCipher.Create(_T("Ciphertext"), wsLbl, CRect(0,0,0,0), this);
	m_edCipher.Create(wsEd | ES_READONLY, CRect(0,0,0,0), this, IDC_EDIT_CIPHER);
}

void CEngimaMachineSimulatorView::PopulateCombos()
{
	m_cbReflector.ResetContent();
	m_cbReflector.AddString(_T("B"));
	m_cbReflector.AddString(_T("C"));

	auto addRotors = [&](CComboBox& cb){
		cb.ResetContent();
		cb.AddString(_T("I"));
		cb.AddString(_T("II"));
		cb.AddString(_T("III"));
		cb.AddString(_T("IV"));
		cb.AddString(_T("V"));
	};
	addRotors(m_cbLeftRotor);
	addRotors(m_cbMidRotor);
	addRotors(m_cbRightRotor);

	auto addAZ = [&](CComboBox& cb){
		cb.ResetContent();
		for (int i=0;i<26;++i){ CString s; s.Format(_T("%c"), TCHAR('A'+i)); cb.AddString(s);} };
	addAZ(m_cbLeftPos); addAZ(m_cbMidPos); addAZ(m_cbRightPos);
	addAZ(m_cbLeftRing); addAZ(m_cbMidRing); addAZ(m_cbRightRing);
}

void CEngimaMachineSimulatorView::LayoutControls()
{
	CRect rc; GetClientRect(&rc);
	int margin =8;
	int line =24;
	int x = margin, y = margin;
	int wLbl =140, wCb =90, gap =8;

	// Row1: Reflector + Plugboard
	m_lblReflector.MoveWindow(x,y,wLbl,line);
	m_cbReflector.MoveWindow(x+wLbl+gap,y,60,line);

	m_lblPlug.MoveWindow(x+wLbl+gap+70, y,220, line);
	m_edPlugboard.MoveWindow(x+wLbl+gap+70+220+gap, y, rc.Width() - (x+wLbl+gap+70+220+gap) - margin -200, line);
	m_btnReset.MoveWindow(rc.Width()-margin-192, y,88, line);
	m_btnRandom.MoveWindow(rc.Width()-margin-96, y,88, line);
	y += line + margin;

	// Row2: Rotors
	m_lblRotors.MoveWindow(x,y,wLbl,line);
	m_cbLeftRotor.MoveWindow(x+wLbl+gap, y, wCb, line);
	m_cbMidRotor.MoveWindow(x+wLbl+gap+wCb+gap, y, wCb, line);
	m_cbRightRotor.MoveWindow(x+wLbl+gap+2*(wCb+gap), y, wCb, line);
	y += line + margin;

	// Row3: Positions
	m_lblPos.MoveWindow(x,y,wLbl,line);
	m_cbLeftPos.MoveWindow(x+wLbl+gap, y, wCb, line);
	m_cbMidPos.MoveWindow(x+wLbl+gap+wCb+gap, y, wCb, line);
	m_cbRightPos.MoveWindow(x+wLbl+gap+2*(wCb+gap), y, wCb, line);
	y += line + margin;

	// Row4: Rings
	m_lblRing.MoveWindow(x,y,wLbl,line);
	m_cbLeftRing.MoveWindow(x+wLbl+gap, y, wCb, line);
	m_cbMidRing.MoveWindow(x+wLbl+gap+wCb+gap, y, wCb, line);
	m_cbRightRing.MoveWindow(x+wLbl+gap+2*(wCb+gap), y, wCb, line);
	y += line + margin;

	// Row5+: Plain/Cipher edits split vertically
	int topEd = y;
	int half = (rc.Height() - topEd - margin*2);
	int edHeight = half/2 - margin/2;
	m_lblPlain.MoveWindow(x, topEd, wLbl, line);
	m_edPlain.MoveWindow(x, topEd+line, rc.Width()-2*margin, edHeight);

	int cy2Top = topEd + line + edHeight + margin;
	m_lblCipher.MoveWindow(x, cy2Top, wLbl, line);
	m_edCipher.MoveWindow(x, cy2Top+line, rc.Width()-2*margin, rc.Height() - (cy2Top+line) - margin);
}

// Update encryption when plaintext or configuration changes
void CEngimaMachineSimulatorView::UpdateCiphertext()
{
	// Build machine from current UI selections
	int reflIdx = m_cbReflector.GetCurSel();
	Reflector refl = (reflIdx==1)? ReflectorC() : ReflectorB();

	auto rotorFromIdx = [](int idx){
		switch(idx){
			case 0: return RotorI();
			case 1: return RotorII();
			case 2: return RotorIII();
			case 3: return RotorIV();
			default: return RotorV();
		}
	};

	Rotor L = rotorFromIdx(m_cbLeftRotor.GetCurSel());
	Rotor M = rotorFromIdx(m_cbMidRotor.GetCurSel());
	Rotor R = rotorFromIdx(m_cbRightRotor.GetCurSel());

	L.setRing(m_cbLeftRing.GetCurSel());
	M.setRing(m_cbMidRing.GetCurSel());
	R.setRing(m_cbRightRing.GetCurSel());

	L.setPosition(m_cbLeftPos.GetCurSel());
	M.setPosition(m_cbMidPos.GetCurSel());
	R.setPosition(m_cbRightPos.GetCurSel());

	Plugboard plug; 
	{
		CString s; m_edPlugboard.GetWindowText(s);
		CT2A a(s);
		plug.configureFromPairs(std::string(a));
	}

	EnigmaMachine em;
	em.setReflector(refl);
	em.setRotors(L,M,R);
	em.setPlugboard(plug);

	// Encrypt
	CString plain; m_edPlain.GetWindowText(plain);
	CT2A ap(plain);
	std::string cipher = em.encrypt(std::string(ap));
	m_edCipher.SetWindowText(CA2T(cipher.c_str()));
}

void CEngimaMachineSimulatorView::ResetSettings()
{
	m_cbReflector.SetCurSel(0); // B
	m_cbLeftRotor.SetCurSel(0); // I
	m_cbMidRotor.SetCurSel(1); // II
	m_cbRightRotor.SetCurSel(2); // III
	m_cbLeftPos.SetCurSel(0);
	m_cbMidPos.SetCurSel(0);
	m_cbRightPos.SetCurSel(0);
	m_cbLeftRing.SetCurSel(0);
	m_cbMidRing.SetCurSel(0);
	m_cbRightRing.SetCurSel(0);
	m_edPlugboard.SetWindowText(_T(""));
	m_edPlain.SetWindowText(_T(""));
	m_edCipher.SetWindowText(_T(""));
}

void CEngimaMachineSimulatorView::RandomizeSettings()
{
	std::random_device rd; std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dAZ(0,25);
	std::uniform_int_distribution<int> dRef(0,1);

	// pick3 distinct rotors from0..4
	std::array<int,5> idx{{0,1,2,3,4}}; std::shuffle(idx.begin(), idx.end(), gen);
	m_cbLeftRotor.SetCurSel(idx[0]);
	m_cbMidRotor.SetCurSel(idx[1]);
	m_cbRightRotor.SetCurSel(idx[2]);

	m_cbLeftRing.SetCurSel(dAZ(gen));
	m_cbMidRing.SetCurSel(dAZ(gen));
	m_cbRightRing.SetCurSel(dAZ(gen));

	m_cbLeftPos.SetCurSel(dAZ(gen));
	m_cbMidPos.SetCurSel(dAZ(gen));
	m_cbRightPos.SetCurSel(dAZ(gen));

	m_cbReflector.SetCurSel(dRef(gen));

	// random plugboard: up to6 pairs
	std::string letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	std::shuffle(letters.begin(), letters.end(), gen);
	int pairs =6;
	std::string pairsStr;
	for (int i=0;i<pairs*2 && i+1 < (int)letters.size(); i+=2)
	{
		pairsStr.push_back(letters[(size_t)i]);
		pairsStr.push_back(letters[(size_t)i+1]);
		pairsStr.push_back(' ');
	}
	m_edPlugboard.SetWindowText(CA2T(pairsStr.c_str()));
}

// Message handlers
void CEngimaMachineSimulatorView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	if (m_cbReflector.GetSafeHwnd())
		LayoutControls();
}

void CEngimaMachineSimulatorView::OnEditPlainChanged()
{
	UpdateCiphertext();
}

void CEngimaMachineSimulatorView::OnConfigChanged()
{
	UpdateCiphertext();
}

void CEngimaMachineSimulatorView::OnComboChanged(UINT /*nID*/)
{
	UpdateCiphertext();
}

void CEngimaMachineSimulatorView::OnResetClicked()
{
	ResetSettings();
	UpdateCiphertext();
}

void CEngimaMachineSimulatorView::OnRandomizeClicked()
{
	RandomizeSettings();
	UpdateCiphertext();
}

// CEngimaMachineSimulatorView drawing

void CEngimaMachineSimulatorView::OnDraw(CDC* /*pDC*/)
{
	CEngimaMachineSimulatorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
}

// Printing and context menu handlers (kept from MFC AppWizard template)
void CEngimaMachineSimulatorView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CEngimaMachineSimulatorView::OnPreparePrinting(CPrintInfo* pInfo)
{
	return DoPreparePrinting(pInfo);
}

void CEngimaMachineSimulatorView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void CEngimaMachineSimulatorView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void CEngimaMachineSimulatorView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CEngimaMachineSimulatorView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}

#ifdef _DEBUG
void CEngimaMachineSimulatorView::AssertValid() const
{
	CView::AssertValid();
}

void CEngimaMachineSimulatorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CEngimaMachineSimulatorDoc* CEngimaMachineSimulatorView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEngimaMachineSimulatorDoc)));
	return (CEngimaMachineSimulatorDoc*)m_pDocument;
}
#endif //_DEBUG
