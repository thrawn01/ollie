#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <KXmlGuiWindow>
#include <KTextEdit>


namespace KTextEditor {
    class Document;
    class View;
}
       
class MainWindow : public KXmlGuiWindow {
    //Q_OBJECT

    public:
        MainWindow(QWidget *parent=0);
        ~MainWindow();

    private:
        KTextEditor::View *m_view;
        KTextEditor::Document *m_document;
};

#endif // MAINWINDOW_H
