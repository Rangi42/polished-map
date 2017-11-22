#include "directory-chooser.h"

#ifdef _WIN32

// utf8towchar and wchartoutf8 copied from Fl_Native_File_Chooser_WIN32.cxx

static LPCWSTR utf8towchar(const char *in) {
	if (in == NULL) { return NULL; }
	WCHAR *wout = NULL;
	int lwout = 0;
	int wlen = MultiByteToWideChar(CP_UTF8, 0, in, -1, NULL, 0);
	if (wlen > lwout) {
		lwout = wlen;
		wout = (WCHAR *)realloc(wout, lwout * sizeof(WCHAR));
	}
	MultiByteToWideChar(CP_UTF8, 0, in, -1, wout, wlen);
	return wout;
}

static const char *wchartoutf8(LPCWSTR in) {
	if (in == NULL) { return NULL; }
	char *out = NULL;
	int lchar = 0;
	int utf8len  = WideCharToMultiByte(CP_UTF8, 0, in, -1, NULL, 0, NULL, NULL);
	if (utf8len > lchar) {
		lchar = utf8len;
		out = (char *)realloc(out, lchar * sizeof(char));
	}
	WideCharToMultiByte(CP_UTF8, 0, in, -1, out, utf8len, NULL, NULL);
	return out;
}

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

	static WCHAR wtitle[256];
	wcsncpy(wtitle, utf8towchar(_title), 256);
	wtitle[255] = '\0';
	_fod_ptr->SetTitle(wtitle);

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
