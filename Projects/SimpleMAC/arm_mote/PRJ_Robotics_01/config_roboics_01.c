/////////////////////////////////////
//  Generated Initialization File  //
/////////////////////////////////////

#include "C8051F340.h"

// Peripheral specific initialization functions,
// Called from the Init_Device() function
void PCA_Init()
{
    PCA0CN    = 0x40;
    PCA0MD    &= ~0x40;
    PCA0MD    = 0x04;
    PCA0CPM0  = 0x42;
    PCA0CPM1  = 0x42;
    PCA0CPM2  = 0x42;
    PCA0CPM3  = 0x42;
    PCA0CPM4  = 0x42;
    PCA0CPH0  = 0xFF;
    PCA0CPH1  = 0xFF;
    PCA0CPH2  = 0xFF;
    PCA0CPH3  = 0xFF;
    PCA0CPH4  = 0xFF;
}

void Timer_Init()
{
    TCON      = 0x50;
    TMOD      = 0x22;
    CKCON     = 0x08;
    TH1       = 0x28;
    TMR2CN    = 0x04;
    TMR2RLL   = 0x6F;
    TMR2RLH   = 0xFE;
    TMR3CN    = 0x04;
    TMR3RLL   = 0x60;
    TMR3RLH   = 0xF0;
}

void UART_Init()
{
    SCON0     = 0x10;
}

void SPI_Init()
{
    SPI0CFG   = 0x60;
    SPI0CN    = 0x09;
    SPI0CKR   = 0x2F;
}

void ADC_Init()
{
    AMX0P     = 0x0A;
    AMX0N     = 0x1F;
    ADC0CN    = 0x80;
}

void Voltage_Reference_Init()
{
    REF0CN    = 0x0F;
}

void Port_IO_Init()
{
    // P0.0  -  SCK  (SPI0), Push-Pull,  Digital
    // P0.1  -  MISO (SPI0), Open-Drain, Digital
    // P0.2  -  MOSI (SPI0), Push-Pull,  Digital
    // P0.3  -  NSS  (SPI0), Push-Pull,  Digital
    // P0.4  -  TX0 (UART0), Push-Pull,  Digital
    // P0.5  -  RX0 (UART0), Open-Drain, Digital
    // P0.6  -  CEX0  (PCA), Open-Drain, Digital
    // P0.7  -  CEX1  (PCA), Open-Drain, Digital

    // P1.0  -  CEX2  (PCA), Open-Drain, Digital
    // P1.1  -  Skipped,     Open-Drain, Analog
    // P1.2  -  CEX3  (PCA), Open-Drain, Digital
    // P1.3  -  Skipped,     Open-Drain, Digital
    // P1.4  -  CEX4  (PCA), Open-Drain, Digital
    // P1.5  -  Skipped,     Open-Drain, Analog
    // P1.6  -  Skipped,     Open-Drain, Digital
    // P1.7  -  Skipped,     Open-Drain, Digital

    // P2.0  -  Unassigned,  Open-Drain, Digital
    // P2.1  -  Unassigned,  Open-Drain, Digital
    // P2.2  -  Unassigned,  Push-Pull,  Digital
    // P2.3  -  Unassigned,  Push-Pull,  Digital
    // P2.4  -  Unassigned,  Open-Drain, Digital
    // P2.5  -  Unassigned,  Open-Drain, Digital
    // P2.6  -  Unassigned,  Open-Drain, Digital
    // P2.7  -  Skipped,     Open-Drain, Digital

    // P3.0  -  Skipped,     Open-Drain, Digital
    // P3.1  -  Skipped,     Open-Drain, Digital
    // P3.2  -  Skipped,     Open-Drain, Digital
    // P3.3  -  Skipped,     Open-Drain, Digital
    // P3.4  -  Skipped,     Open-Drain, Digital
    // P3.5  -  Skipped,     Open-Drain, Digital
    // P3.6  -  Unassigned,  Push-Pull,  Digital
    // P3.7  -  Skipped,     Open-Drain, Digital

    P1MDIN    = 0xDD;
    P0MDOUT   = 0x1D;
    P2MDOUT   = 0x0C;
    P3MDOUT   = 0x40;
    P1SKIP    = 0xEA;
    P2SKIP    = 0x80;
    P3SKIP    = 0xBF;
    XBR0      = 0x03;
    XBR1      = 0x45;
}

void Oscillator_Init()
{
    int i = 0;
    OSCLCN    |= 0x82;
    CLKMUL    = 0x80;
    for (i = 0; i < 20; i++);    // Wait 5us for initialization
    CLKMUL    |= 0xC0;
    while ((CLKMUL & 0x20) == 0);
    CLKSEL    = 0x03;
    OSCICN    = 0x83;
}

void Interrupts_Init()
{
    IP        = 0x10;
    EIE1      = 0x88;
    IE        = 0xF0;
}

// Initialization function for device,
// Call Init_Device() from your main program
void Init_Device(void)
{
    PCA_Init();
    Timer_Init();
    UART_Init();
    SPI_Init();
    ADC_Init();
    Voltage_Reference_Init();
    Port_IO_Init();
    Oscillator_Init();
    Interrupts_Init();
}
