#include "directory-chooser.h"

#ifdef _WIN32

Directory_Chooser::Directory_Chooser(int) : _title(NULL), _filename(NULL), _fod_ptr(NULL) {}

Directory_Chooser::~Directory_Chooser() {
	if (_fod_ptr) { _fod_ptr->Release(); }
	free((void *)_filename);
}

int Directory_Chooser::show() {
	HRESULT hr;

	if (!_fod_ptr) {
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void **>(&_fod_ptr));
		if (!SUCCEEDED(hr)) { return -1; }
	}
	free((void *)_filename);
	_filename = NULL;

	WCHAR *wtitle = utf8towchar(_title);
	_fod_ptr->SetTitle(wtitle);
	free(wtitle);

	FILEOPENDIALOGOPTIONS fod_opts;
	_fod_ptr->GetOptions(&fod_opts);
	fod_opts |= FOS_PICKFOLDERS;
	_fod_ptr->SetOptions(fod_opts);

	HWND hWnd = GetForegroundWindow();
	hr = _fod_ptr->Show(hWnd);
	if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED)) { return 1; }
	if (!SUCCEEDED(hr)) { return -1; }

	IShellItem *pItem;
	hr = _fod_ptr->GetResult(&pItem);
	if (!SUCCEEDED(hr)) { return -1; }

	PWSTR pszFilePath;
	hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
	if (!SUCCEEDED(hr)) { pItem->Release(); return -1; }

	_filename = wchartoutf8(pszFilePath);
	CoTaskMemFree(pszFilePath);
	pItem->Release();
	return 0;
}

#endif
