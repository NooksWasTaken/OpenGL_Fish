#include <wx/wx.h>
#include <wx/glcanvas.h>
#include <wx/sound.h> 
#include <GL/gl.h>
#include <GL/glu.h>

class MyGLCanvas : public wxGLCanvas {
public:
    MyGLCanvas(wxWindow* parent)
        : wxGLCanvas(parent, wxID_ANY, nullptr, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE),
        m_gaugeValue(10), m_points(0) {
        m_context = new wxGLContext(this);
        SetBackgroundStyle(wxBG_STYLE_PAINT);

        // Set the background color to grey (RGB: 0.5, 0.5, 0.5)
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

        // Bind events
        Bind(wxEVT_PAINT, &MyGLCanvas::OnPaint, this);
        Bind(wxEVT_SIZE, &MyGLCanvas::OnResize, this);
    }

    ~MyGLCanvas() {
        delete m_context;
    }

    void DecreaseGauge() {
        if (m_gaugeValue > 0) {
            --m_gaugeValue;
        }
        if (m_gaugeValue == 0) {
            m_points += 200; // sAdd points when the gauge reaches 0
            m_gaugeValue = 10; // Resets bar
        }
        Refresh(); // Redraw the canvas
    }

    // Getter functions to access points and gauge value
    int GetPoints() const { return m_points; }
    int GetGaugeValue() const { return m_gaugeValue; }

private:
    void OnPaint(wxPaintEvent& event) {
        wxPaintDC dc(this);
        SetCurrent(*m_context);

        // Clear the OpenGL canvas
        glClear(GL_COLOR_BUFFER_BIT);
        glLoadIdentity();

        // Get the current canvas size
        int width, height;
        GetClientSize(&width, &height);

        // Draw light blue rectangle (smaller and positioned higher)
        glColor3f(0.678f, 0.847f, 0.902f); // Light blue color
        glBegin(GL_QUADS);
        glVertex2f(-0.6f, 0.6f);  // Top-left
        glVertex2f(0.6f, 0.6f);   // Top-right
        glVertex2f(0.6f, 0.2f);   // Bottom-right
        glVertex2f(-0.6f, 0.2f);  // Bottom-left
        glEnd();

        // Draw the gauge bar (positioned lower)
        float gaugeWidth = (1.6f * m_gaugeValue) / 10.0f; // Width scales with gauge value
        glColor3f(1.0f, 0.0f, 0.0f); // Red
        glBegin(GL_QUADS);
        glVertex2f(-0.8f, -0.2f);           // Top-left
        glVertex2f(-0.8f + gaugeWidth, -0.2f); // Top-right
        glVertex2f(-0.8f + gaugeWidth, -0.3f); // Bottom-right
        glVertex2f(-0.8f, -0.3f);           // Bottom-left
        glEnd();

        // Swap buffers to render
        SwapBuffers();
    }

    void OnResize(wxSizeEvent& event) {
        // Adjust the OpenGL viewport when the window is resized
        int width, height;
        GetClientSize(&width, &height);
        glViewport(0, 0, width, height);
        Refresh();
    }

    wxGLContext* m_context;
    int m_gaugeValue;
    int m_points;
};

class MyFrame : public wxFrame {
public:
    MyFrame()
        : wxFrame(nullptr, wxID_ANY, "FISH TANK'D", wxDefaultPosition, wxSize(800, 600)) {

        // Create a vertical box sizer
        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

        // Create the OpenGL canvas
        m_canvas = new MyGLCanvas(this);
        sizer->Add(m_canvas, 1, wxEXPAND);

        // Create a horizontal sizer for the button and point text
        wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);

        // Creates the button
        wxButton* button = new wxButton(this, wxID_ANY, "FISH");
        button->Bind(wxEVT_BUTTON, &MyFrame::OnButtonClicked, this);

        // Creates the points counter text
        m_pointsText = new wxStaticText(this, wxID_ANY, "Points Earned: 0", wxDefaultPosition, wxSize(150, 30), wxALIGN_LEFT);

        // Creates the gauge bar value text
        m_gaugeText = new wxStaticText(this, wxID_ANY, "Health: 10", wxDefaultPosition, wxSize(150, 30), wxALIGN_LEFT);

        // Adds the points text to the right of the button
        buttonSizer->Add(m_gaugeText, 0, wxALIGN_LEFT | wxRIGHT, 10);
        buttonSizer->Add(button, 0, wxALIGN_CENTER);
        buttonSizer->Add(m_pointsText, 0, wxALIGN_RIGHT | wxLEFT, 10);

        // Adds the button sizer to the main sizer
        sizer->Add(buttonSizer, 0, wxALIGN_CENTER | wxBOTTOM, 10);

        // Sets the sizer for the frame
        SetSizer(sizer);

        // Loads sound file
        m_sound = new wxSound("C:\\Users\\Michael Jerome Reyes\\source\\repos\\OpenGL_Game\\OpenGL_Game\\FISH.wav"); //Insert random .wav file
    }

    ~MyFrame() {
        delete m_sound;
    }

private:
    void OnButtonClicked(wxCommandEvent& event) {
        m_canvas->DecreaseGauge();
        UpdateUI();
        PlaySound();  // Plays sound when button is clicked
    }

    void UpdateUI() {
        // Updates the points counter
        m_pointsText->SetLabel("Points Earned: " + std::to_string(m_canvas->GetPoints()));

        // Updates the health bar display
        m_gaugeText->SetLabel("Health: " + std::to_string(m_canvas->GetGaugeValue()));
    }

    void PlaySound() {
        if (m_sound->IsOk()) {
            m_sound->Play(wxSOUND_ASYNC);
        }
    }

    MyGLCanvas* m_canvas;
    wxStaticText* m_pointsText;
    wxStaticText* m_gaugeText;
    wxSound* m_sound;
};

class MyApp : public wxApp {
public:
    virtual bool OnInit() {
        MyFrame* frame = new MyFrame();
        frame->Show();
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);
