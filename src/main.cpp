/*
    This file ( main.cpp ) is part of MinimalSerialMonitor.
    (C) 31.1.2021 Fidoriel

    MinimalSerialMonitor is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    MinimalSerialMonitor is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MinimalSerialMonitor.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "wx/wx.h"
#include "wx/grid.h"
#include "wx/sizer.h"
#include "wx/panel.h"
#include "wx/file.h"
#include "wx/event.h"
#include <string>
#include <stdio.h>
#include "wx/filedlg.h"

#include "../icons/AppIcon.xpm"

#if defined( __linux__ ) || defined( __FreeBSD__ ) || defined( __APPLE__ )
    #include <dirent.h>
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include "serial_unix.h"
#endif

#if defined( WIN32 )
    #include <string>
    #include <Windows.h>
    #include "serial_win.h"
#endif

class MyApp : public wxApp
{
public:
  virtual bool OnInit();
};

class MyFrame : public wxFrame
{
    public:
        MyFrame();
        ~MyFrame();
        void OnClear( wxCommandEvent& event );
        void OnRefreshSerial( wxCommandEvent& event );
        void OnSend( wxCommandEvent& event );
        void OnCon( wxCommandEvent& event );
        void OnFreeze( wxCommandEvent& event );
        void OnIdle( wxIdleEvent& event );
        void RefreshSerial();
        void loadPorts();
        void OnNewLineChange( wxCommandEvent& event );
        void OnExit( wxCommandEvent& event );
        void OnSave( wxCommandEvent& event );
        void OnKeyPress( wxKeyEvent& event );
        void Send();

        wxMenuBar* menuBar;
        wxMenu* fileMenu;
        wxMenu* helpMenu;

        wxBoxSizer* topSizer;

        wxPanel* mainPanel;
        wxBoxSizer* sendSizer;
        wxTextCtrl* toSend;
        wxButton* sendBtn;

        wxTextCtrl* rcvData;

        wxFlexGridSizer* conSizer;
        wxChoice* portCh;
        wxChoice* baudCh;
        wxButton* conBtn;

        wxStatusBar* statBar;

        wxArrayString portAry;
        wxArrayString baudAry;
        wxButton* freezeBtn;
        wxButton* clearBtn;
        wxButton* refreshBtn;
        wxArrayString freezeBuffer;
        wxChoice* lineEnd;
        wxArrayString lineReturnAry;
        wxString lineEndStr;

        wxArrayString history;
        size_t currentPosHist;

        bool isConnected;
        bool isFreeze;
        bool isLineReturn;
        int connection;

        char port[128];
        char* portPoint;

        enum
        {
            ID_RefreshSerial,
            ID_Clear,
            ID_Send,
            ID_Freeze,
            ID_Connect,
            ID_NewLineChange,
            ID_VisitGithub
        };
    private:
        DECLARE_EVENT_TABLE();
};

wxIMPLEMENT_APP(MyApp);

wxBEGIN_EVENT_TABLE( MyFrame, wxFrame )
    EVT_BUTTON( ID_RefreshSerial, MyFrame::OnRefreshSerial )
    EVT_BUTTON( ID_Clear, MyFrame::OnClear )
    EVT_BUTTON( ID_Send, MyFrame::OnSend )
    EVT_BUTTON( ID_Freeze, MyFrame::OnFreeze )
    EVT_BUTTON( ID_Connect, MyFrame::OnCon )
    EVT_IDLE( MyFrame::OnIdle )
    EVT_CHOICE( ID_NewLineChange, MyFrame::OnNewLineChange )
    EVT_MENU( wxID_EXIT, MyFrame::OnExit )
    EVT_MENU( wxID_SAVE, MyFrame::OnSave )
wxEND_EVENT_TABLE()

bool MyApp::OnInit()
{
    MyFrame* m_frame = new MyFrame();
    m_frame->Show();
    SetAppName( "MinimalSerialMonitor" );
    SetVendorName( "fidoriel" );
    return true;
}

MyFrame::MyFrame() : wxFrame( NULL, wxID_ANY, wxGetApp().GetAppName(), wxDefaultPosition, wxSize( 640, 480 ) )
{
    helpMenu = new wxMenu;
    helpMenu->Append( wxID_ABOUT );
    helpMenu->Append( ID_VisitGithub, "GitHub Development Page", "" );
    // helpMenu->Append( wxID_PREFERENCES );
    helpMenu->Append( wxID_ABOUT );

    Bind( wxEVT_COMMAND_MENU_SELECTED,
        []( wxCommandEvent& ) {
        
        //Blöng sound
        wxBell();

        //Message window
        wxString msg;
        msg.Printf( "%s build with %s is a simple Serial Monitor, only offering the most importand functions, avaliable for Linux, macOS and Windows, including Raspberry Pi OS. ", wxGetApp().GetAppName(), wxVERSION_STRING );
        wxString title;
        title.Printf( "About %s", wxGetApp().GetAppName() );
        wxMessageBox( msg, title, wxOK | wxICON_INFORMATION );

        }, wxID_ABOUT );

    Bind( wxEVT_COMMAND_MENU_SELECTED,
        []( wxCommandEvent& ) {

        wxLaunchDefaultBrowser( "https://github.com/fidoriel/MinimalSerialMonitor" );

        }, ID_VisitGithub );

    fileMenu = new wxMenu;
    fileMenu->Append( wxID_SAVE, "Save Output" );
    fileMenu->Append( wxID_EXIT, "Close" );

    menuBar = new wxMenuBar();
    menuBar->Append( fileMenu, "File" );
    menuBar->Append( helpMenu, "Help" );
    this->SetMenuBar( menuBar );


    this->isLineReturn = true;
    lineEndStr = "\n";
    SetIcon( wxICON( AppIcon ) );

    mainPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    topSizer = new wxBoxSizer( wxVERTICAL );

    sendSizer = new wxBoxSizer( wxHORIZONTAL );
    sendBtn = new wxButton( mainPanel, ID_Send, "Send", wxDefaultPosition, wxSize( 95, -1 ) );
    toSend = new wxTextCtrl( mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize );
    sendSizer->Add( toSend, 1, wxALL | wxEXPAND, 5);
    sendSizer->Add( sendBtn, 0, wxALL, 5 );

    toSend->Bind( wxEVT_KEY_DOWN, &MyFrame::OnKeyPress, this );

    rcvData = new wxTextCtrl( mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
    rcvData->SetEditable( false );

    this->isConnected = false;
    baudAry.Add( "300" );
    baudAry.Add( "1200" );
    baudAry.Add( "2400" );
    baudAry.Add( "4800" );
    baudAry.Add( "9600" );
    baudAry.Add( "14400" );
    baudAry.Add( "19200" );
    baudAry.Add( "38400" );
    baudAry.Add( "57600" );
    baudAry.Add( "115200" );
    
    conSizer = new wxFlexGridSizer( 4, 0, 0 );
    portCh = new wxChoice( mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, portAry );
    this->RefreshSerial();
    if ( portCh->GetCount() > 0 )
        portCh->SetSelection( portCh->GetCount() - 1 );

    baudCh = new wxChoice( mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, baudAry );
    baudCh->SetStringSelection( "9600" );
    conBtn = new wxButton( mainPanel, ID_Connect, "Connect", wxDefaultPosition, wxSize( 95, -1) );
    refreshBtn = new wxButton( mainPanel, ID_RefreshSerial, "Refresh", wxDefaultPosition, wxSize( 95, -1) );
    clearBtn = new wxButton( mainPanel, ID_Clear, "Clear", wxDefaultPosition, wxSize( 95, -1) );

    lineReturnAry.Add( "none" );
    lineReturnAry.Add( "new Line" );
    lineReturnAry.Add( "CR" );

    lineEnd = new wxChoice( mainPanel, ID_NewLineChange, wxDefaultPosition, wxDefaultSize, lineReturnAry );
    lineEnd->SetSelection( 1 );
    freezeBtn = new wxButton( mainPanel, ID_Freeze, "Freeze", wxDefaultPosition, wxSize( 95, -1) );

    conSizer->AddGrowableCol( 0 );
    
    conSizer->Add( portCh, 1, wxALL | wxEXPAND, 5 );
    conSizer->Add( refreshBtn, 0, wxALL, 5 );
    conSizer->Add( baudCh, 1, wxALL | wxEXPAND, 5 );
    conSizer->Add( conBtn, 0, wxALL, 5 );
    conSizer->Add( lineEnd, 1, wxALL | wxEXPAND, 5 );
    conSizer->Add( freezeBtn, 0, wxALL, 5 );
    conSizer->Add( clearBtn, 0, wxALL, 5 );

    topSizer->Add( sendSizer, 0, wxALL | wxEXPAND, 0 );
    topSizer->Add( rcvData, 1, wxALL | wxEXPAND, 5 );
    topSizer->Add( conSizer, 0, wxALL | wxEXPAND, 0 );

    statBar = new wxStatusBar( this, wxID_ANY );
    statBar->SetFieldsCount( 2 ); 

    this->SetStatusBar( statBar );
    this->SetStatusText( "Not Connected", 0 );

    mainPanel->SetSizerAndFit( topSizer );
    mainPanel->Layout();
    
    this->Center();
    this->SetMinSize( this->GetBestSize() );
}


void MyFrame::OnClear( wxCommandEvent& event )
{
    rcvData->Clear();
}

void MyFrame::OnRefreshSerial( wxCommandEvent& event )
{
    this->RefreshSerial();
}

void MyFrame::RefreshSerial()
{
    this->loadPorts();
    portCh->Clear();
    for (size_t i = 0; i < portAry.GetCount(); i++)
    {
        portCh->Append( portAry.Item( i ) );
    }
}

void MyFrame::loadPorts()
{
    #if defined( __linux__ ) || defined( __FreeBSD__ ) || defined( __APPLE__ )
        this->portAry.Empty();
        int comportsElements;
        char directory_name[  ] = "/dev/";
        DIR *ptr;
        struct dirent *directory;
        ptr = opendir( directory_name );
        while( ( directory = readdir( ptr ) ) != NULL )
        {
            #if defined( __APPLE__ )
            if ( strstr( directory->d_name, "cu." ) != NULL )
            #endif
            #if defined( __linux__ ) || defined( __FreeBSD__ )
            if ( strstr( directory->d_name, "ttyUSB" ) != NULL )
            #endif
            {
                char tmp[ 100 ] = "/dev/";
                for ( int i = 0; i < 95; i++ )
                {
                    tmp[ 5+i ] = directory->d_name[ i ];
                }
                this->portAry.Add( tmp );
            }
        }
    #endif

    #if defined( WIN32 )
        this->portAry.Empty();
        wchar_t lpTargetPath[ 4096 ];

        for ( int i = 0; i < 255; i++ )
        {
            std::wstring str = L"COM" + std::to_wstring( i );
            DWORD res = QueryDosDevice( str.c_str(), lpTargetPath, 5000 );

            if ( res != 0 )
            {
                this->portAry.Add( str );
            }
            if ( ::GetLastError() == ERROR_INSUFFICIENT_BUFFER )
            {
            }
        }
    #endif
}

void MyFrame::OnSend( wxCommandEvent& event )
{
    this->Send();
}

void MyFrame::Send()
{
    if ( this->isConnected && !this->toSend->GetValue().IsEmpty())
    {
        std::string serialSignal = std::string( this->toSend->GetValue().mb_str() );
        char sendSignal[ 4096 ];

        copy( serialSignal.begin(), serialSignal.end(), sendSignal );

        sendSignal[ serialSignal.size() ] = 0;
        
        int rtnv = write_port( this->connection, sendSignal );

        if ( rtnv != 0)
            this->SetStatusText( "Write Succesful", 1 );
        else
            this->SetStatusText( "Write Error", 1 );

        this->currentPosHist = history.GetCount();
        this->history.Add( this->toSend->GetValue() );
        this->toSend->Clear();
    }
}

void MyFrame::OnCon( wxCommandEvent& event )
{
    if ( !this->isConnected )
    {
        close_port( this->connection );
        std::string stdString = std::string( this->portCh->GetStringSelection().mb_str() );
        strcpy( port, stdString.c_str() );
        this->portPoint = port;
        
        this->connection = connect_port( portPoint, wxAtoi( this->baudCh->GetStringSelection() ) );

        if ( this->connection > -1 )
        {
            this->isConnected = true;
            this->SetStatusText( "Connected", 0 );
            conBtn->SetLabel( "Disconnect" );
        }
    }
    else
    {
        close_port( this->connection );
        this->SetStatusText( "Not Connected", 0 );
        this->isConnected = false;
        conBtn->SetLabel( "Connect" );
    }
}

void MyFrame::OnFreeze( wxCommandEvent& event )
{
    if ( this->isFreeze )
    {
        this->isFreeze = false;
        this->freezeBtn->SetLabel( "Freeze" );

        for (size_t i = 0; i < this->freezeBuffer.GetCount(); i++)
        {
            rcvData->AppendText( freezeBuffer.Item( i ) );
        }

        freezeBuffer.Clear();
        
    }
    else
    {
        this->isFreeze = true;
        this->freezeBtn->SetLabel( "Catch Up" );
    }
}

void MyFrame::OnIdle( wxIdleEvent& event )
{
    if ( this->isConnected )
    {
        unsigned char rcv[ 8192 ];
        int bytes = read_port( this->connection, rcv, 8192 );
        if( bytes > 0 )
        {
            rcv[ bytes ] = 0;          
            wxString appStr;
            appStr.Printf( "%s", ( char * )rcv );
            appStr << lineEndStr;

            if ( !this->isFreeze )
            {              
                this->rcvData->AppendText( appStr );
            }

            else if ( this->isFreeze )
            {
                freezeBuffer.Add( appStr );
            }
        }
    }
}

void MyFrame::OnNewLineChange( wxCommandEvent& event )
{
    if ( lineEnd->GetSelection() == 0 )
        this->lineEndStr = "";
    if ( lineEnd->GetSelection() == 1 )
        this->lineEndStr = "\n";
    if ( lineEnd->GetSelection() == 2 )
        this->lineEndStr = "\r";
}

void MyFrame::OnExit( wxCommandEvent& event )
{
    close_port( this->connection );
    this->Close( true );
}

void MyFrame::OnSave( wxCommandEvent& event )
{
    wxFileDialog saveDialog( this, "Save Output", wxEmptyString, "SerialOut", "txt files ( *.txt )|*.txt", wxFD_SAVE | wxFD_OVERWRITE_PROMPT );
    if ( saveDialog.ShowModal() == wxID_OK )
    {
        rcvData->SaveFile( saveDialog.GetPath() );
    }
    else
    {
        return;
    }
}

MyFrame::~MyFrame()
{
    close_port( this->connection );
}

void MyFrame::OnKeyPress( wxKeyEvent& event )
{
    event.Skip();

    if ( history.GetCount() == 0 )
        return;

    switch ( event.GetKeyCode() )
    {
        case WXK_UP:
        case WXK_NUMPAD_UP:
            if ( this->currentPosHist > 0 )
                this->currentPosHist -= 1;
            
            toSend->SetValue( history.Item( this->currentPosHist ) );
            event.Skip( false );
            break;
        
        case WXK_DOWN:
        case WXK_NUMPAD_DOWN:
            this->currentPosHist += 1;

            if ( this->currentPosHist >= history.GetCount() )
            {
                this->currentPosHist = history.GetCount();
                toSend->Clear();
            }

            else
            {
                toSend->SetValue( history.Item( this->currentPosHist ) );
            }

            event.Skip( false );
            break;

        case WXK_RETURN:
        case WXK_NUMPAD_ENTER:
            this->Send();
            event.Skip( false );
            break;
    }
}