/*
 * QT6 Hello World Application for MetalOS
 * 
 * This is a minimal QT6 application that will run full-screen
 * on MetalOS with Radeon RX 6600 GPU support.
 */

#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QFont>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Create main window
    QWidget window;
    window.setWindowTitle("MetalOS - QT6 Hello World");
    
    // Create layout
    QVBoxLayout *layout = new QVBoxLayout(&window);
    
    // Create "Hello World" label
    QLabel *label = new QLabel("Hello, World!");
    QFont font = label->font();
    font.setPointSize(48);
    font.setBold(true);
    label->setFont(font);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("QLabel { color: white; background-color: black; }");
    
    // Create info label
    QLabel *infoLabel = new QLabel(
        "MetalOS - A minimal OS for QT6 applications\n"
        "AMD64 + Radeon RX 6600 GPU\n"
        "UEFI Native Boot"
    );
    infoLabel->setAlignment(Qt::AlignCenter);
    infoLabel->setStyleSheet("QLabel { color: #00FF00; background-color: black; }");
    
    // Add widgets to layout
    layout->addWidget(label);
    layout->addWidget(infoLabel);
    
    // Set window properties
    window.setLayout(layout);
    window.setStyleSheet("QWidget { background-color: black; }");
    
    // Show full screen
    window.showFullScreen();
    
    return app.exec();
}
