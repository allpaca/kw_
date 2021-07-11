#include "kwapi.h"

#include <atlconv.h>
#include <string>
#include <ciso646>
#include <atlcomcli.h>
#include <sstream>
using namespace std;

#define B2W(bstr) _wcsdup(bstr)
#define B2A(bstr) _com_util::ConvertBSTRToString(bstr)

///////////////////////////////////////////////////////////////////////////////

kwapi::kwapi() : event_(*this){
	HRESULT hr = OleInitialize(nullptr);
	if (SUCCEEDED(hr)) {
		oleInitialized_ = true;
	}
}

kwapi::~kwapi() {
	if (oleInitialized_) {
		OleUninitialize();
		oleInitialized_ = false;
	}
}

bool kwapi::create() {

	CComPtr<IOleObject> oleObj;
	HRESULT hr = OleCreate(__uuidof(KHOpenAPI), IID_IOleObject,
		OLERENDER_NONE, nullptr, this, this, (void**)&oleObj);

	if (FAILED(hr)) {
		return false;
	}

	hr = oleObj->SetClientSite(this);

	//hr = OleSetContainedObject(oleObj, TRUE);
	// oleObj->SetHostNames(L"test", L"test");
	hr = oleObj->DoVerb(OLEIVERB_OPEN, nullptr, this, 0, 0, nullptr);
	if (FAILED(hr)) {
		return false;
	}

	HWND hControlWnd = FindWindow(_T("AfxWnd100"), _T("KHOpenAPI Control"));
	if (hControlWnd) {
		ShowWindow(hControlWnd, SW_HIDE);
	}
	oleObj->QueryInterface(&v);

	CComQIPtr<IConnectionPointContainer, &IID_IConnectionPointContainer> cpc;

	hr = v->QueryInterface(IID_IConnectionPointContainer, (void**)&cpc);
	if (FAILED(hr)) {
		return false;
	}

	CComPtr<IConnectionPoint> cp;
	hr = cpc->FindConnectionPoint(__uuidof(_DKHOpenAPIEvents), &cp);
	if (FAILED(hr)) {
		return false;
	}

	hr = cp->Advise(&event_, &event_.cookie());


	//HMODULE hDll = LoadLibrary("axKiwoom.dll");
	/*
	USES_CONVERSION;
	CLSID clsid;

	LPOLESTR progid = (LPOLESTR)L"KHOPENAPI.KHOpenAPICtrl";
	hr = CLSIDFromProgID(progid, &clsid);
	//if (FAILED(hr)) return;

	_DKHOpenAPIPtr api;

	hr = CoCreateInstance(__uuidof(KHOpenAPI), nullptr, CLSCTX_INPROC_SERVER,
	__uuidof(_DKHOpenAPI), (void**)&api);

	try
	{
		//auto path = api->GetAPIModulePath();
		//api->CommConnect();

	}
	catch (_com_error& e) {
	}

	while (true) {
		MSG msg;
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	*/

	//auto path = api->GetAPIModulePath();

	//SetCurrentDirectory(path);

	return true;
}

COMAPI kwapi::QueryInterface(REFIID riid, void** ppvObject) {
	// Always set out parameter to NULL, validating it first.
	if (!ppvObject) return E_INVALIDARG;
	*ppvObject = nullptr;

	if (riid == IID_IUnknown or
		riid == IID_IOleClientSite or
		riid == IID_IOleInPlaceSite or riid == IID_IStorage) {
		// Increment the reference count and return the pointer.
		*ppvObject = (LPVOID)this;
		AddRef();
		return NOERROR;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) kwapi::AddRef(void) {
	InterlockedIncrement(&refCount_);
	return refCount_;
}

STDMETHODIMP_(ULONG) kwapi::Release(void) {
	const ULONG cnt = InterlockedDecrement(&refCount_);
	if (cnt == 0) {
		delete this;
	}
	return cnt;
}

COMAPI kwapi::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker,
	IMoniker** ppmk) {
	if ((dwAssign == OLEGETMONIKER_ONLYIFTHERE) and
		(dwWhichMoniker == OLEWHICHMK_CONTAINER)) {
		return E_FAIL;
	}
	return E_NOTIMPL;
}

COMAPI kwapi::GetContainer(IOleContainer** ppContainer) {

	return E_NOINTERFACE;
}

COMAPI kwapi::ShowObject(void) {
	return S_OK;
}

COMAPI kwapi::OnShowWindow(BOOL fShow) {
	return S_OK;
}

COMAPI kwapi::GetWindow(HWND* phwnd) {
	return E_NOTIMPL;
}

COMAPI kwapi::CanInPlaceActivate(void) {
	return S_OK;
}

COMAPI kwapi::SetClass(REFCLSID clsid) {
	return S_OK;
}

HRESULT kwapi::onInvoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags,
	DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo,
	UINT* puArgErr) {
	if (!pDispParams) return E_INVALIDARG;

	VARIANTARG* varg = pDispParams->rgvarg;
	switch (dispIdMember)
	{
	case 1:
	{
		_bstr_t sSplmMsg = varg[0].bstrVal;
		_bstr_t sMessage = varg[1].bstrVal;
		_bstr_t sErrorCode = varg[2].bstrVal;
		long nDataLength = varg[3].lVal;
		_bstr_t sPrevNext = varg[4].bstrVal;
		_bstr_t sRecordName = varg[5].bstrVal;
		_bstr_t sTrCode = varg[6].bstrVal;
		_bstr_t sRQName = varg[7].bstrVal;
		_bstr_t sScrNo = varg[8].bstrVal;
		
		if (onReceiveTrDataW_) {
			onReceiveTrDataW_(sScrNo, sRQName, sTrCode, sRecordName,
				sPrevNext, nDataLength, sErrorCode, sMessage, sSplmMsg);
		}
		
		if (onReceiveTrDataA_) {
			onReceiveTrDataA_(sScrNo, sRQName, sTrCode, sRecordName,
				sPrevNext, nDataLength, sErrorCode, sMessage, sSplmMsg);
		}
	}break;
	case 2:
	{
		_bstr_t sRealData = varg[0].bstrVal;
		_bstr_t sRealType = varg[1].bstrVal;
		_bstr_t sRealKey = varg[2].bstrVal;
		
		if (onReceiveRealDataW_) {
			onReceiveRealDataW_(sRealKey, sRealType, sRealData);
		}

		if (onReceiveRealDataA_) {
			onReceiveRealDataA_(sRealKey, sRealType, sRealData);
		}
	}break;
	case 3:
	{
		_bstr_t sScrNo(varg[3].bstrVal);
		_bstr_t sRQName(varg[2].bstrVal);
		_bstr_t sTrCode(varg[1].bstrVal);
		_bstr_t sMsg(varg[0].bstrVal);

		if (onReceiveMsgW_) {
			onReceiveMsgW_(sScrNo, sRQName, sTrCode, sMsg);
		}

		if (onReceiveMsgA_) {
			onReceiveMsgA_(sScrNo, sRQName, sTrCode, sMsg);
		}
	}break;
	case 4:
	{
		_bstr_t sFidList(varg[0].bstrVal);
		const long nItemCnt = varg[1].lVal;
		_bstr_t sGubun(varg[2].bstrVal);
		
		if (onReceiveChejanDataW_) {
			onReceiveChejanDataW_(sGubun, nItemCnt, sFidList);
		}

		if (onReceiveChejanDataA_) {
			onReceiveChejanDataA_(sGubun, nItemCnt, sFidList);
		}
	}break;
	case 5:
	{
		LONG errCode = varg[0].lVal;
		if (onEventConnect_) {
			onEventConnect_(errCode);
		}
	}break;
	case 7: 
	{
		_bstr_t sTrCode(varg[3].bstrVal);
		_bstr_t strType(varg[2].bstrVal);
		_bstr_t strConditionName(varg[1].bstrVal);
		_bstr_t strConditionIndex(varg[0].bstrVal);

		if (onReceiveRealConditionW_) {
			onReceiveRealConditionW_(sTrCode, strType, strConditionName, 
				strConditionIndex);
		}

		if (onReceiveRealConditionA_) {
			onReceiveRealConditionA_(sTrCode, strType, strConditionName,
				strConditionIndex);
		}
	}break;
	case 8:
	{
		_bstr_t sScrNo(varg[4].bstrVal);
		_bstr_t strCodeList(varg[3].bstrVal);
		_bstr_t strConditionName = varg[2].bstrVal;
		int nIndex = varg[1].intVal;
		int nNext = varg[0].intVal;

		if (onReceiveTrConditionW_) {
			onReceiveTrConditionW_(sScrNo, strCodeList, strConditionName, 
				nIndex, nNext);
		}

		if (onReceiveTrConditionA_) {
			onReceiveTrConditionA_(sScrNo, strCodeList, strConditionName,
				nIndex, nNext);
		}
	}break;
	case 9: {
		long lRet = varg[1].lVal;
		_bstr_t sMsg(varg[0].bstrVal);

		if (onReceiveConditionVerW_) {
			onReceiveConditionVerW_(lRet, sMsg);
		}

		if (onReceiveConditionVerA_) {
			onReceiveConditionVerA_(lRet, sMsg);
		}
	}break;
	default:
		break;
	}
	return S_OK;
}

COMAPI kwevent::QueryInterface(REFIID riid, void** ppvObject) {
	// Always set out parameter to NULL, validating it first.
	if (!ppvObject) return E_INVALIDARG;
	*ppvObject = nullptr;

	if (riid == __uuidof(_DKHOpenAPIEvents) or riid == IID_IDispatch
		or riid == IID_IUnknown) {
		*ppvObject = this;
		AddRef();
		return NOERROR;
	}
	return E_NOINTERFACE;
}

///////////////////////////////////////////////////////////////////////////////
COMAPI kwevent::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags,
	DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo,
	UINT* puArgErr) {
	return api_.onInvoke(dispIdMember, riid, lcid, wFlags, pDispParams, pVarResult, 
		pExcepInfo, puArgErr);
}

long kwapi::commConnect() {
	if (!v) return KWFAIL;
	return v->CommConnect();
}
