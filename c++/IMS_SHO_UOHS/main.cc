#include "simlib.h"
#include "time.h"

// Casy simulacie
#define POCET_ROKOV 10
#define DLZKA_SIMULACIE 365*POCET_ROKOV

// Casy generacii
#define CAS_VEREJNE_ZAKAZKY 0.6
#define CAS_HS_FUZE 7.3
#define CAS_HS_KARTELY 122
#define CAS_HS_DOMINANCE 365

// Typy rozhodnutia pri HS
enum hs_typy
{
    FUZE,
    KARTEL,
    DOMINANCIA
};

// Predseda uradu
Facility Verejne_zakazky_predseda("Verejne zakazky - predseda");

// Verejne zakazky
Store Zam_verejne_zakazky("Zamestnanci - verejne zakazky", 75 * 10); // 75 * 10
Store Zam_verejne_zakazky_predseda("Zamestnanci od predsedu - verejne zakazky", 12 * 10); //12 * 3

// Hospodarska sutaz
Store Zam_hs_fuze("Zamestnanci - HS - Fuze", 7 * 2);
Store Zam_hs_kartely("Zamestnanci - HS - Kartely", 12 * 2);
Store Zam_hs_dominance("Zamestnanci - HS - Dominance", 15  * 2);
Store Zam_hs_druhy_stupen("Zamestnanci - HS - druhy stupen", 6  * 10);

// Verejne zakazky
Histogram TableVZCelkovyCas("VZ - celkovy cas",0, 10, DLZKA_SIMULACIE/10);
Histogram TableVZPrichody("VZ - prichody za rok", 0, DLZKA_SIMULACIE/POCET_ROKOV, POCET_ROKOV);
Histogram TableVZOdchody("VZ - odchody za rok", 0, DLZKA_SIMULACIE/POCET_ROKOV, POCET_ROKOV);
Histogram TableVZRozklad("VZ - ide do rozkladu", 0, DLZKA_SIMULACIE/POCET_ROKOV, POCET_ROKOV);
Histogram TableVZSudy("VZ - ide k sudu", 0, DLZKA_SIMULACIE/POCET_ROKOV, POCET_ROKOV);
Histogram TableVZ1stupen("VZ - zahajene 1.stupnove", 0, DLZKA_SIMULACIE/POCET_ROKOV, POCET_ROKOV);

// Hospodarska sutaz
Histogram TableHSFuzeCelkovyCas("HS - Fuze - celkovy cas",0, 10, DLZKA_SIMULACIE/10);
Histogram TableHSKartelyCelkovyCas("HS - Kartely - celkovy cas",0, 10, DLZKA_SIMULACIE/10);
Histogram TableHSDominanceCelkovyCas("HS - Dominance - celkovy cas",0, 10, DLZKA_SIMULACIE/10);

Histogram TableDebug("DEBUUUUUUUUUUUUUUUUUUUUUUUUG",0, 10, DLZKA_SIMULACIE/10);

Histogram TableHSFuzePrichody("HS - Fuze - prichody za rok", 0, DLZKA_SIMULACIE/POCET_ROKOV, POCET_ROKOV);
Histogram TableHSKartelyPrichody("HS - Kartely - prichody za rok", 0, DLZKA_SIMULACIE/POCET_ROKOV, POCET_ROKOV);
Histogram TableHSDominancePrichody("HS - Dominance - prichody za rok", 0, DLZKA_SIMULACIE/POCET_ROKOV, POCET_ROKOV);
Histogram TableHSFuzeOdchody("HS - Fuze - odchody za rok", 0, DLZKA_SIMULACIE/POCET_ROKOV, POCET_ROKOV);
Histogram TableHSKartelyOdchody("HS - Kartely - odchody za rok", 0, DLZKA_SIMULACIE/POCET_ROKOV, POCET_ROKOV);
Histogram TableHSDominanceOdchody("HS - Dominance - odchody za rok", 0, DLZKA_SIMULACIE/POCET_ROKOV, POCET_ROKOV);


// Verejne zakazky
int prvyDen = 0;
int podnetCislo = 0;
int podnetNeukonceny = 0;

// Hospodarska sutaz


//
// Verejne zakazky - podnet
//
class Podnet : public Process
{ 
    double Prichod;
    int podnetLocal;

    void Behavior()
    {
        Prichod = Time;             
        podnetLocal = podnetCislo;
        podnetCislo++;
        podnetNeukonceny++;

        TableVZPrichody(Time);

        //Print("Podnet %d vstupil\n", podnetLocal);

        // Podnet si zoberie zamestnanca
        Enter(Zam_verejne_zakazky, 1);

        // Delenie Von vs 1.stupnove rizeni
        if(Random() <= 0.12)
        {
            // Vratime zamestnanca
            Leave(Zam_verejne_zakazky, 1);

            // Opustime system
            // return;

        }
        else
        {
            // 88% ze ide na 1. stupnove rizeni


            // Cakame exp(60dni) na rozhodnutie
            Wait(Exponential(60));

            TableVZ1stupen(Time);

            // 51% ide von zo systemu, zvysok na rozklad
            if(Random() <= 0.51)
            {
                // Vratime zamestnanca
                Leave(Zam_verejne_zakazky, 1);

                // Opustime system
                // return;
            }
            else
            {
                // 49% ide na rozklad
                TableVZRozklad(Time);

                // Vratime klasickeho zamestnanca
                Leave(Zam_verejne_zakazky, 1);

                // Tu zoberiem predsedu, aby si pozrel podnet
                Seize(Verejne_zakazky_predseda);
                Wait(Exponential(1));
                Release(Verejne_zakazky_predseda);

                // 6% predseda vyhodi
                if(Random() <= 0.06)
                {
                    // Vyjdeme zo systemu
                    // return;
                }
                else
                {
                    // 94% ide k rozhodnutiu

                    // Zoberieme zamestnanca od predsedu
                    Enter(Zam_verejne_zakazky_predseda, 1);

                    // Seize(Verejne_zakazky_rozhodnutie2);
                    Wait(Exponential(60));
                    // Release(Verejne_zakazky_rozhodnutie2);

                    // Podla rozhodnutia, ide 86% von zo systemu
                    if(Random() <= 0.86)
                    {
                        // Vratime zamestnanca
                        Leave(Zam_verejne_zakazky_predseda, 1);
                        // return;
                    }
                    else
                    {
                        // 14% ide na sud
                        TableVZSudy(Time);

                        Wait(Uniform(182, 1825));

                        // Odovzdame zamestnanca
                        Leave(Zam_verejne_zakazky_predseda, 1);
                        // return;
                    }
                }

            }

        }

        //Print("Podnet %d vystupil\n", podnetLocal);
        podnetNeukonceny--;
        TableVZCelkovyCas(Time-Prichod);        
        TableVZOdchody(Time);

    }
};

//
// Hospodarska sutaz - rozhodnutie
//
class Rozhodnutie : public Process
{ 
    double Prichod;

    public:
        int typ;
        double casPrichodu;

    void Behavior()
    {
        Prichod = Time;             

        // Predseda sa pozrie na podnet
        Seize(Verejne_zakazky_predseda);
        Wait(Exponential(1));
        Release(Verejne_zakazky_predseda);

        // 76% ide k rozkladu
        if(Random() <= 0.76)
        {
            // Zoberieme si zamestnanca
            Enter(Zam_hs_druhy_stupen, 1);

            // Ideme na rozklad, ktory trva exp 214 dni
            Wait(Exponential(214));
            //Print("- %f\n", Normal(214, 100));
            //Wait(Uniform(100, 300));
            TableDebug(Time-Prichod);

            // 34% ide von zo systemu
            if(Random() <= 0.34)
            {
                // Vratime zamestnanca a odchadzame zo systemu
                Leave(Zam_hs_druhy_stupen, 1);
            }
            else
            {
                // 66% ide k sudu
                // Sud trva rovnomerne 1095 az 1825 dni
                Wait(Uniform(1095, 1825));

                // Vratime zamestnanca a ideme von
                Leave(Zam_hs_druhy_stupen, 1);
            }

        }
        else
        {
            // 24% ide von zo ystemu
        }

        switch(typ)
        {
            case FUZE:
                TableHSFuzeCelkovyCas(Time-casPrichodu);
                TableHSFuzeOdchody(Time);
            break;
            case KARTEL:
                TableHSKartelyCelkovyCas(Time-casPrichodu);
                TableHSKartelyOdchody(Time);
            break;
            case DOMINANCIA:
                TableHSDominanceCelkovyCas(Time-casPrichodu);
                TableHSDominanceOdchody(Time);
            break;
        }
    }
};

//
// Hospodarska sutaz - Fuze
//
class Fuze : public Process
{ 
    double Prichod;

    void Behavior()
    {
        Prichod = Time;             

        TableHSFuzePrichody(Time);

        // Zoberieme si zamestnanca
        Enter(Zam_hs_fuze, 1);

        // 50% ide k zjednodusenemu riadeniu
        if(Random() <= 0.5)
        {
            // zjednodusene riadenie trva 20 dni
            Wait(20);
        }
        else
        {
            // standardne riadenie trva 30 dni
            Wait(30);
        }

        // 25% ide k rozhodnutiu
        if(Random() <= 0.25)
        {
            // Vratime zamestnanca
            Leave(Zam_hs_fuze, 1);


            // Ideme k rozhodnutiu
            Rozhodnutie *a = new Rozhodnutie;
            a->typ = FUZE;
            a->casPrichodu = Prichod;
            a->Activate();
        }
        else
        {
            // 75% ide von zo systemu
            Leave(Zam_hs_fuze, 1);
            TableHSFuzeOdchody(Time);
            TableHSFuzeCelkovyCas(Time-Prichod);
        }


    }
};

//
// Hospodarska sutaz - Kartely
//
class Kartely : public Process
{ 
    double Prichod;

    void Behavior()
    {
        Prichod = Time;             

        TableHSKartelyPrichody(Time);

        Enter(Zam_hs_kartely, 1);

        // Riesi sa to exp 210 dni
        Wait(Exponential(210));

        // 75% ide von zo systemu
        if(Random() <= 0.75)
        {
            // Vratime zamestnanca
            Leave(Zam_hs_kartely, 1);
            TableHSKartelyCelkovyCas(Time-Prichod);
            TableHSKartelyOdchody(Time);
        }
        else
        {
            // 25% ide k rozhodnutiu

            // Vratime zamestnanca
            Leave(Zam_hs_kartely, 1);

            // Ide k rozhodnutiu
            Rozhodnutie *a = new Rozhodnutie;
            a->typ = KARTEL;
            a->casPrichodu = Prichod;
            a->Activate();
        }

        TableHSKartelyCelkovyCas(Time-Prichod);

    }
};

//
// Hospodarska sutaz - Dominance
//
class Dominance : public Process
{ 
    double Prichod;

    void Behavior()
    {
        Prichod = Time;             

        TableHSDominancePrichody(Time);

        Enter(Zam_hs_dominance, 1);

        // Riesi sa to exp 210 dni
        Wait(Exponential(210));

        // 75% ide von zo systemu
        if(Random() <= 0.75)
        {
            // Vratime zamestnanca
            Leave(Zam_hs_dominance, 1);
            TableHSDominanceCelkovyCas(Time-Prichod);
            TableHSDominanceOdchody(Time);
        }
        else
        {
            // 25% ide k rozhodnutiu

            // Vratime zamestnanca
            Leave(Zam_hs_dominance, 1);

            // Ide k rozhodnutiu
            Rozhodnutie *a = new Rozhodnutie;
            a->typ = DOMINANCIA;
            a->casPrichodu = Prichod;
            a->Activate();
        }

        TableHSDominanceCelkovyCas(Time-Prichod);
    }
};

//
// Generovanie podnetov
//
class Gen_verejne_zakazky : public Event
{
    void Behavior()
    {            
        (new Podnet)->Activate();
        Activate(Time + Exponential(CAS_VEREJNE_ZAKAZKY));
    }
};

class Gen_hs_fuze : public Event
{
    void Behavior()
    {            
        (new Fuze)->Activate();
        Activate(Time + Exponential(CAS_HS_FUZE));
    }
};

class Gen_hs_kartely : public Event
{
    void Behavior()
    {            
        (new Kartely)->Activate();
        Activate(Time + Exponential(CAS_HS_KARTELY));
    }
};

class Gen_hs_dominance : public Event
{
    void Behavior()
    {            
        (new Dominance)->Activate();
        Activate(Time + Exponential(CAS_HS_DOMINANCE));
    }
};

int main()
{
    Print("IMS %d\n", Time);
    SetOutput("stats.out");
    RandomSeed(time(NULL));
    prvyDen = Time;

    Init(0, DLZKA_SIMULACIE);

    (new Gen_verejne_zakazky)->Activate(Time + Exponential(CAS_VEREJNE_ZAKAZKY));
    (new Gen_hs_fuze)->Activate(Time + Exponential(CAS_HS_FUZE));
    (new Gen_hs_kartely)->Activate(Time + Exponential(CAS_HS_KARTELY));
    (new Gen_hs_dominance)->Activate(Time + Exponential(CAS_HS_DOMINANCE));

    Run();

    Print("Testujem pre obdobie %d dni\n", DLZKA_SIMULACIE);
    Verejne_zakazky_predseda.Output();

    // Verejne zakazky
    //Print("Do systemu prislo %d podnetov\n", podnetCislo);
    //Print("Neukoncilo sa %d podnetov\n", podnetNeukonceny);

    Print("/*********************************************/\n");
    Print("/**                                         **/\n");
    Print("/** Verejne zakazky                         **/\n");
    Print("/**                                         **/\n");
    Print("/*********************************************/\n");

    Zam_verejne_zakazky.Output();
    Zam_verejne_zakazky_predseda.Output();
    
    TableVZPrichody.Output();
    TableVZOdchody.Output();
    TableVZRozklad.Output();
    TableVZ1stupen.Output();
    TableVZSudy.Output();
    //TableVZCelkovyCas.Output();

    Print("/*********************************************/\n");
    Print("/**                                         **/\n");
    Print("/** Hospodarska sutaz                       **/\n");
    Print("/**                                         **/\n");
    Print("/*********************************************/\n");

    // Hospodarska sutaz
    Zam_hs_fuze.Output();
    Zam_hs_kartely.Output();
    Zam_hs_dominance.Output();
    Zam_hs_druhy_stupen.Output();

    TableHSFuzePrichody.Output();
    TableHSFuzeOdchody.Output();
    TableHSKartelyPrichody.Output();
    TableHSKartelyOdchody.Output();
    TableHSDominancePrichody.Output();
    TableHSDominanceOdchody.Output();

    //TableHSFuzeCelkovyCas.Output();
    //TableHSKartelyCelkovyCas.Output();
    //TableHSDominanceCelkovyCas.Output();

    return 0;
}

