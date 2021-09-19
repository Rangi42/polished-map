#include "directory-chooser.h"

#ifdef _WIN32

Directory_Chooser::Directory_Chooser(int) : _title(NULL), _filename(NULL), _fod_ptr(NULL) {}

Directory_Chooser::~Directory_Chooser() {
	if (_fod_ptr) { _fod_ptr->Release(); }
	delete [] _filename;
}

int Directory_Chooser::show() {
	HRESULT hr;

	if (!_fod_ptr) {
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void **>(&_fod_ptr));
		if (!SUCCEEDED(hr)) { return -1; }
	}
	delete [] _filename;
	_filename = NULL;

	int wlen = MultiByteToWideChar(CP_UTF8, 0, _title, -1, NULL, 0);
	WCHAR *wtitle = new WCHAR[wlen];
	MultiByteToWideChar(CP_UTF8, 0, _title, -1, wtitle, wlen);
	_fod_ptr->SetTitle(wtitle);
	delete [] wtitle;

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

	int len = WideCharToMultiByte(CP_UTF8, 0, pszFilePath, -1, NULL, 0, NULL, NULL);
	char *filename = new char[len];
	WideCharToMultiByte(CP_UTF8, 0, pszFilePath, -1, filename, len, NULL, NULL);
	_filename = filename;

	CoTaskMemFree(pszFilePath);
	pItem->Release();
	return 0;
}

#endif
