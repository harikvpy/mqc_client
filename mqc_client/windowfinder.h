#pragma once

#include <bitset>
#include <list>
#include <vector>
/**
 * Searches the window hierarchy looking for a window that matches
 * certain criteria. Matching criteria can be specified as a text
 * string which is then parsed by the class.
 */

/**
    window SecAlertWindow {
      text: "Security Alert"
      id: 1000
      class: #Dialog
      pid: $curprocessid
      tid: $curthreadid
      parent: $desktop
      children:
	    window YesButton
	    {
	      text: "Yes"
	      id: 1001
	      class: Button
	    }
	    window NoButton
	    {
	      text: "No"
	      id: 1002
	      class: Button
	    }
	    window ViewCertButton
	    {
	      text: "View Certificate"
	      id: 1003
	      class: Button
	    }
    }

"{text="Security alert", id=100, class="mainwndclass", parent=$desktop, style=0x03a, exstyle=0x90, numchildren=3, 
    children=[{id=10001}, {id=10002}, {id=10004}]}"

 */
class WindowFinder {

	HWND parenthandle_;	// parent window
	long id_;		// id
	std::wstring text_;	// text
	std::wstring classname_;	// window classname
	unsigned style_;
	unsigned extendedstyle_;
	int processid_;
	int threadid_;

	std::list<WindowFinder> children_;

	// the enum if used only to index into the bitset
	enum Flags {
		parent=0,
		id,
		text,
		classname,
		style,
		extendedstyle,
		processid,
		threadid,

		flagcount
	};

    class Matcher {
        WindowFinder* finder_;
	    long id_;		// id
	    std::wstring text_;	// text
	    std::wstring classname_;	// window classname
	    unsigned style_;
	    unsigned extendedstyle_;
	    int processid_;
	    int threadid_;

        std::list<Matcher> children_;
        
        HWND searchtarget_;

    public:
        Matcher(WindowFinder* _finder, long _id, wchar_t const* _text=0, wchar_t const* _class=0, 
                unsigned _style=0, unsigned _exstyle=0, int _pid=0, int _tid=0)
            : parenthandle_(0), id_(_id), text_(_text?_text:L""), classname_(_class?_class:L""), 
              style_(_style), extendedstyle_(_exstyle), processid_(_pid), threadid_(_tid),
              /*fields_(flagcount), */searchtarget_(NULL)
        {}
    };
    friend class Matcher;
    Matcher matcher_;

	std::bitset<flagcount> fields_;

    HWND searchtarget_;
public:
    WindowFinder(HWND _parent, long _id, wchar_t const* _text=0, wchar_t const* _class=0, 
        unsigned _style=0, unsigned _exstyle=0, int _pid=0, int _tid=0)
        : parenthandle_(_parent), id_(_id), text_(_text?_text:L""), classname_(_class?_class:L""), 
          style_(_style), extendedstyle_(_exstyle), processid_(_pid), threadid_(_tid),
          fields_(flagcount), searchtarget_(NULL)
    {
        fields_[parent] = 1;
        fields_[id] = 1;
        fields_[text] = text_.length() ? 1 : 0;
    }

    ~WindowFinder()
    {}

    HWND Find()
    {
        ::EnumChildWindows(parenthandle_, WindowFinder::_EnumProc, reinterpret_cast<LPARAM>(this));
        return searchtarget_;
    }

    BOOL EnumProc(HWND hWnd)
    {
        // id
        if (id_ != ::GetWindowLong(hWnd, GWL_ID))
            return TRUE;

        // text
        if (fields_[text]) {
            int textlen = ::GetWindowTextLength(hWnd);
            // check for empty text
            if (text_.length() == 0 && textlen != 0)
                return TRUE; // no match, keep searching

            if (textlen > 0) {
                std::wstring text(textlen+1, L'\0');
                ::GetWindowText(hWnd, const_cast<wchar_t*>(text.c_str()), textlen);
                if (::wcscmp(text_.c_str(), text.c_str()) != 0)
                    return TRUE;
            }
        }

        // classname
        if (fields_[classname] && classname_.length()>0) {
            wchar_t wndclassname[256] = {0};
            ::GetClassName(hWnd, wndclassname, (int)_countof(classname)-1);
            if (::wcscmp(classname_.c_str(), wndclassname) != 0)
                return TRUE; // no match
        }

        // window style
        if (fields_[style]) {
            if (::GetWindowLong(hWnd, GWL_STYLE)&style_ != style_)
                return TRUE;
        }

        // window extended style
        if (fields_[extendedstyle]) {
            if (::GetWindowLong(hWnd, GWL_EXSTYLE)&extendedstyle_ != extendedstyle_)
                return TRUE; // no match
        }

        // process id
        if (fields_[processid]) {
            DWORD dwProcessId = 0;
            ::GetWindowThreadProcessId(hWnd, &dwProcessId);
            if (processid_ != dwProcessId)
                return TRUE; // no match
        }

        // thread id
        if (fields_[threadid]) {
            if (threadid_ != ::GetWindowThreadProcessId(hWnd, NULL))
                return TRUE; // no match
        }

        // if children are specified, check if they match the requirements
        if (children_.size()) {
            std::list<
        }

        // all criteria match! we have found our window!
        searchtarget_ = hWnd;
        return FALSE;
    }

    static BOOL CALLBACK _EnumProc(HWND hWnd, LPARAM lParam)
    {
        return reinterpret_cast<WindowFinder*>(lParam)->EnumProc(hWnd);
    }
};
