#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "FOSSA-Comms.h"

#include <iostream>

#include <QtCore>

static uint8_t g_Key[16];



MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->LoadKeyFromFile();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::LoadKeyFromFile()
{
    FILE* f = fopen("key.txt", "r");

    if (f == nullptr)
    {
        throw std::runtime_error("Could not find key.txt");
    }

    for (int i = 0; i < 32; i+=2)
    {
        char asciiCharacterA = getc(f);
        char asciiCharacterB = getc(f);
        char byteHexStr[3];

        byteHexStr[0] = asciiCharacterA;
        byteHexStr[1] = asciiCharacterB;
        byteHexStr[2] = '\0';

        uint8_t hexValueAsByte= (uint8_t)strtol(byteHexStr, NULL, 16);
        g_Key[i/2] = hexValueAsByte;
    }

    fclose(f);

    ui->statusbar->showMessage("Success: AES Key loaded from key.txt file.", 5000);
}
