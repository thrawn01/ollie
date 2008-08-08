#include <KTextEditor/Document>
#include <KTextEditor/View>
#include <KTextEditor/Editor>
#include <KTextEditor/EditorChooser>
#include <KMessageBox>
#include <KLocale>
#include <KApplication>

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : KXmlGuiWindow(parent), m_view(0), m_document(0) {

    // Choose our KTextEditor implementation
    KTextEditor::Editor *editor = KTextEditor::EditorChooser::editor();

    if (!editor) {
        KMessageBox::error(this, i18n("A KDE text-editor component could not be found;\n"
                                    "please check your KDE installation."));
        kapp->exit(1);
    }

    m_document = editor->createDocument(0);
    m_view = qobject_cast<KTextEditor::View*>(m_document->createView(this));

    setCentralWidget(m_view);

    //setupGUI();
}

MainWindow::~MainWindow() {
    if (m_document) {
      // the document deletes its views itself
      delete m_document;
    }
}

