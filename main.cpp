#include <fstream>
#include <iostream>
class Kurs;
using namespace std;

class Student {
    // private attribute aus Sicherheitsgruenden
private:
    string name;
    string vorname;
    int matnr;
    Kurs *kurs; //Verweiß auf den eingeschriebenen Kurs
    Student *next; // Next Referenz für die LinkedList

public:
    // Konstruktor um Werte zu initalisieren
    Student() : matnr(0), kurs(nullptr), next(nullptr) {
    }
    // Hier wird der Kurspointer gesetzt und somit auf den eingeschriebenen Kurs verwiesen
    void applyKurs(Kurs *kurs);

    // Getter Methode
    Student *getNext() const {
        return next;
    }
    Kurs *getKurs() const {
        return kurs;
    }

    // Setter Methode
    void setNext(Student *next) {
        this->next = next;
    }
    // Friend Deklarationen damit die Operatoren Zugriff auf Klasseninterne Inhalte haben (private Attribute)
    friend ostream &operator<<(ostream &os, const Student &student);

    friend istream &operator>>(istream &is, Student &student);

    friend bool operator==(const Student &, const Student &);

    // Die Klasse Verwaltung bekommt hiermit ebenso Zugriff
    friend class Verwaltung;
};

bool operator==(const Student &a, const Student &b) {
    return a.matnr == b.matnr && a.name == b.name && a.vorname == b.vorname;
}

// Ueberschreibung des Output operators
ostream &operator<<(ostream &os, const Student &s) {
    os << "Name: " << s.name << ", Vorname: " << s.vorname << ", Matrikelnummer: " << s.matnr;
    return os;
}

// Ueberschreibung des Input Operators und kurs+next Pointer mit nullptr initialisieren
istream &operator>>(istream &is, Student &student) {
    cout << "Name: ";
    is >> student.name;
    cout << "Vorname: ";
    is >> student.vorname;
    cout << "Matrikelnummer: ";
    is >> student.matnr;
    student.kurs = nullptr;
    student.next = nullptr;
    return is;
}
// Impl der Forward Deklaration
void Student::applyKurs(Kurs *kurs) {
    if (this->kurs == nullptr) {
        this->kurs = kurs;
    } else {
        // Exception wird hier geworfen im Fehlerfall damit das Programm nicht abstuerzt. Wird an anderer Stelle wieder gefangen.
        throw invalid_argument("Student ist schon in einem Kurs registriert");
    }
}

class Kurs {
private:
    //static const ints um das Array zu initalisieren, und um die Begrenzungen der Aufgabe zu setzen
    static const int min = 3;
    static const int max = 8;
    Student *s[max]; // Array von Verweisen auf Schueler um sie dem Kurs zuzuordnen
    int teilnehmerzahl;
    string name;
    Kurs *next; // Next Pointer fuer die LinkedList

public:
    // Kontruktor und direkte Zuweisung im Methodenkopf als auch das ganze Pointerarray auf nullptr initalisieren
    Kurs(const string &name): teilnehmerzahl(0), name(name), next(nullptr) {
        for (int i = 0; i < max; i++) s[i] = nullptr;
    }

    friend ostream &operator<<(ostream &os, const Kurs &kurs);

    friend istream &operator>>(istream &is, Kurs &kurs);

    friend bool operator==(const Kurs &, const Kurs &);

    // Getter Methoden
    string getName() const {
        return name;
    }

    int getTeilnehmerzahl() const {
        return teilnehmerzahl;
    }

    // Setter Methoden
    void setName(const string &name) {
        this->name = name;
    }

    void setTeilnehmerzahl(int teilnehmerzahl) {
        this->teilnehmerzahl = teilnehmerzahl;
    }

    // Student wird hier dem Kurs hinzugefuegt. Nimmt einen Pointer entgegen und schreibt ihn in das Array, inkrementiert ebenso die teilnehmerzahl
    void addStudent(Student *student) {
        if (teilnehmerzahl < max) {
            try {
                student->applyKurs(this);
                this->s[teilnehmerzahl] = student;
                teilnehmerzahl++;
                // Hier wird die Exception gefangen und weitergegeben um die zentral zu behandeln
            } catch (invalid_argument&) {
                throw;
            }
            // Ebenso im Fehlerfall eine Exception werfen
        } else {
            throw invalid_argument("Kurs ist voll");
        }
    }

    // Hier wird ein Student aus dem Kurs entfernt
    void removeStudent(const Student *student) {
        // Fehlerabfrage ob der Pointer nullptr ist
        if (student == nullptr) {
            throw invalid_argument("Student ist null");
        }
        // Fehlerabfrage ob dieser Schueler auch wirklich in dem Kurs eingeschrieben ist
        if (student->getKurs()->getName() != name) {
            throw invalid_argument("Student ist nicht im Kurs registriert");
        }

        int index = -1;
        // Finde den Index des zu löschenden Studenten
        for (int i = 0; i < teilnehmerzahl; i++) {
            // Hier wird die == Ueberladung von Schueler benutzt
                if (*s[i] == *student) {
                index = i;
                break;
            }
        }

        // Wenn Student gefunden wurde
        if (index != -1) {
            // Letztes Element an Position des gelöschten Elements verschieben
            s[index] = s[teilnehmerzahl - 1];

            // Letztes Element auf nullptr setzen
            s[teilnehmerzahl - 1] = nullptr;
            // Teilnehmerzahl dekrementieren
            teilnehmerzahl--;
        } else {
            throw invalid_argument("Student nicht im Kurs-Array gefunden");
        }
    }
    // Studenten ausgeben und die << Ueberladung benutzen
    void printStudents() const {
        for (int i = 0; i < teilnehmerzahl; i++) {
            if (s[i] != nullptr) {
                cout << "Student " << i + 1 << ": " << *s[i] << endl;
            } else {
                cout << "Student " << i + 1 << ": <nicht verfügbar>" << endl;
            }
        }
    }
    // Getter Methoden
    int getFreiePlaetze() const {
        return max - teilnehmerzahl;
    }

    Kurs *getNext() const {
        return next;
    }
    // Setter Methoden
    void setNext(Kurs *next) {
        this->next = next;
    }
};
// Ueberladung des << Operators um einen Kurs auszugeben. Gleichzeitig wird hier noch die Meldung ausgegeben ob der Kurs ausgebucht oder oder nicht statt findet
ostream &operator<<(ostream &os, const Kurs &k) {
    os << "Kursname: " << k.name << ", Aktuelle Teilnehmerzahl: " << k.teilnehmerzahl << " von: " << k.max << "\n" <<
            endl;
    if (k.teilnehmerzahl > 0) {
        os << "Alle registrierten Studenten: " << "\n" << endl;
        k.printStudents();
        if (k.teilnehmerzahl == k.max) {
            os << "Kurs bereits ausgebucht" << endl;
        }
    }
    if (k.teilnehmerzahl < k.min) {
        os << "Kurs findet nicht statt - zu wenig Teilnehmer" << endl;
    }
    return os;
}

bool operator==(const Kurs &a, const Kurs &b) {
    return a.name == b.name;
}
// Klasse Verwaltung um die Listen zu verwalten. Hier wird der Headpointer von Studenten als auch Kursen verwaltet. Somit sind hier beide LinkedLists verfügbar
class Verwaltung {
private:
    Kurs *headKurs;
    Student *headStudent;

public:
    // Konstruktor
    Verwaltung() : headKurs(nullptr), headStudent(nullptr) {
    }
    // Dekonstruktor, damit der Speicherplatz wieder freigegeben wird und keine Memory Leaks entstehen. Es werden einmal beide LinkedLists durch iteriert und die Referenzen deleted.
    ~Verwaltung() {
        Kurs *current = headKurs;
        while (current != nullptr) {
            Kurs *nextKurs = current->getNext();
            delete current;
            current = nextKurs;
        }

        Student *current2 = headStudent;
        while (current2 != nullptr) {
            Student *nextStudent = current2->getNext();
            delete current2;
            current2 = nextStudent;
        }
    }
    // Kurssuche, gibt Pointer auf gesuchten Kurs zurück oder wirf eine Exception im Fehlerfall
    Kurs *findKurs(const string &name) const {
        Kurs *current = headKurs;
        while (current != nullptr) {
            Kurs *nextKurs = current->getNext();
            if (current->getName() == name) {
                return current;
            }
            current = nextKurs;
        }
        throw invalid_argument("Kurs wurde nicht gefunden");
    }

    // Studentensuche, gibt Pointer auf gesuchten Studenten zurück oder wirf eine Exception im Fehlerfall
    Student *findStudent(const Student &s) const {
        Student *current = headStudent;
        while (current != nullptr) {
            Student *nextStudent = current->getNext();
            if (*current == s) {
                return current;
            }
            current = nextStudent;
        }
        throw invalid_argument("Student wurde nicht gefunden");
    }

// Hier wird alles persistiert.
    void speichernInDatei(const string &dateiname) {
        ofstream datei(dateiname);

        // Fehlerabfangen falls der Zugriff auf die Datei scheitert
        if (!datei.is_open()) {
            cerr << "Fehler beim Oeffnen der Datei zum Schreiben!" << endl;
            return;
        }
        // Ueber Linked List iterieren und Inhalt strukturiert um es gut wieder einzulesen speichern
        Student *currentStudent = headStudent;
        while (currentStudent != nullptr) {
            datei << "STUDENT_NAME: " << currentStudent->name << endl;
            datei << "STUDENT_VORNAME: " << currentStudent->vorname << endl;
            datei << "STUDENT_MATNR: " << currentStudent->matnr << endl;
            datei << "STUDENT_KURS: " << currentStudent->kurs->getName() << "\n" << endl;
            currentStudent = currentStudent->getNext();
        }

        datei.close();
        cout << "Daten wurden in " << dateiname << " gespeichert." << endl;
    }
    // Wieder einlesen aus der Datei
    void ausDateiLesen(const string &dateiname) {
        ifstream datei;
        datei.open(dateiname, ios::in);

        // Fehlerabfangen falls der Zugriff auf die Datei scheitert
        if (!datei.is_open()) {
            cerr << "Fehler beim Öffnen der Datei zum Lesen!" << endl;
            return;
        }

        string zeile;

        string studentName;
        string studentVorname;
        string studentKurs;
        int studentMatnr = 0;
        // while schleife um bis zum End Of File zu iterieren und alles einzulesen
        while (!datei.eof()) {
            // Hier wird jede Zeile aus der Datei in dem string zeile gespeichert
            getline(datei, zeile);
            // Leere Zeilen ueberspringen
            if (zeile.empty()) continue;
            // : Als Ziel suchen, da links davon label rechts davon wert steht
            int pos = zeile.find(':');
            string label = zeile.substr(0, pos);
            string wert = zeile.substr(pos + 2); // +2 um das Leerzeichen nach dem : zu überspringen

            // Je nach Wert einlesen
            if (label == "STUDENT_NAME") {
                studentName = wert;
                cout << "Student Name: " << studentName << endl;
            } else if (label == "STUDENT_VORNAME") {
                studentVorname = wert;
                cout << "Student Vorname: " << studentVorname << endl;
            } else if (label == "STUDENT_MATNR") {
                studentMatnr = stoi(wert);
                cout << "Student Matrikelnummer: " << studentMatnr << endl;
            } else if (label == "STUDENT_KURS") {
                studentKurs = wert;
                cout << "Student Kurs: " << studentKurs << endl;

                // Hier Datenstruktur herstellen und verknüpfen
                Student *neuerStudent = new Student();

                neuerStudent->name = studentName;
                neuerStudent->vorname = studentVorname;
                neuerStudent->matnr = studentMatnr;

                addStudent(neuerStudent);

                try {
                    Kurs *k = findKurs(studentKurs);
                    k->addStudent(neuerStudent);
                } catch (const invalid_argument &e) { // Fehlerabfangen und ausgeben mit Fehlermeldung
                    cerr << "Fehler: " << e.what() << endl;
                }
            }
        }

        datei.close();
        cout << "Daten wurden aus " << dateiname << " geladen." << endl;
    }

    // Fügt einen Studenten zur verketteten Liste hinzu.
    // Wenn die Liste leer ist, wird der Student als Kopf gesetzt.
    // Ansonsten wird der neue Student vorne eingefügt.
    void addStudent(Student *student) {
        if (student != nullptr && headStudent == nullptr) {
            headStudent = student;
        } else if (student != nullptr) {
            student->setNext(headStudent);
            headStudent = student;
        } else {
            throw invalid_argument("Student ist null");
        }
    }
    // Fügt einen Kurs zur verketteten Liste hinzu.
    // Analog zur Studentliste: Kurs wird vorne eingefügt.
    void addKurs(Kurs *kurs) {
        if (kurs != nullptr && headKurs == nullptr) {
            headKurs = kurs;
        } else if (kurs != nullptr) {
            kurs->setNext(headKurs);
            headKurs = kurs;
        } else {
            throw invalid_argument("Kurs ist null");
        }
    }
    // Gibt den Kopf der Kursliste zurück.
    // Da der Rückgabewert nicht verändert wird, ist die Methode const.
    Kurs *getHeadKurs() const {
        return headKurs;
    }
    // Gibt alle Studenten auf der Konsole aus.
    // Wenn keine Studenten vorhanden sind, entsprechende Ausgabe.
    void printAllStudents() const {
        const Student *current = headStudent;
        if (current == nullptr) {
            cout << "Keine Studenten vorhanden" << endl;
        }
        while (current != nullptr) {
            cout << "Student " << *current << endl;
            current = current->getNext();
        }
    }
    // Gibt alle Kurse mit Trennlinie aus.
    // Wirft Fehler, wenn keine Kurse vorhanden sind.
    void printAllCourses() const {
        const Kurs *current = headKurs;

        if (headKurs == nullptr) {
            throw invalid_argument("Keine Kurse vorhanden");
        }

        while (current != nullptr) {
            cout << *current << endl;
            cout << "---------------------------------------------" << endl;
            current = current->getNext();
        }
    }
    // Gibt alle Kursnamen aus, erlaubt Benutzerauswahl,
    // und gibt den ausgewählten Kurs zurück.
    Kurs *printAllCourseNamesAndGiveSelection() const {
        Kurs *current = headKurs;

        // Fehlerfall abfangen, wenn keine Kurse vorhanden sind
        if (headKurs == nullptr) {
            throw invalid_argument("Keine Kurse vorhanden");
        }

        int i = 1;

        // Durchlauf durch alle Kurse, um sie mit fortlaufender Nummer auszugeben
        while (current != nullptr) {
            cout << i++ << ": " << current->getName() << endl;
            current = current->getNext();
        }

        int auswahl;

        // Benutzereingabe zur Auswahl eines Kurses
        cout << "Bitte Kurs auswaehlen (1-" << (i - 1) << "): ";
        cin >> auswahl;

        // Validierung der Benutzereingabe
        while (auswahl < 1 || auswahl > (i - 1)) {
            cout << "Ungueltige Auswahl! Bitte erneut versuchen (1-" << (i - 1) << "): ";
            cin >> auswahl;
        }

        // Kurszeiger zurücksetzen auf Anfang
        current = headKurs;

        // Bis zur ausgewählten Kursposition iterieren
        for (int j = 1; j < auswahl; j++) {
            current = current->getNext();
        }

        // Zeiger auf ausgewählten Kurs zurückgeben
        return current;
    }


    // Gibt nur Kurse aus, die noch freie Plätze haben
    void printAllNotFullCourses() const {
        const Kurs *current = headKurs;

        while (current != nullptr) {
            if (current->getFreiePlaetze() > 0) {
                cout << *current << endl;
                cout << "---------------------------------------------" << endl;
            }
            current = current->getNext();
        }
    }

    // Entfernt einen Studenten aus der Liste sowie aus dem zugehörigen Kurs
    void deleteStudent(const Student &student) {
        Student *current = headStudent;

        // Entferne den Studenten aus dem Kurs
        Kurs *deleteStudentInKurs = student.kurs;
        deleteStudentInKurs->removeStudent(&student);

        // Fehler, wenn keine Studenten vorhanden
        if (headStudent == nullptr) {
            throw invalid_argument("Keine Studenten vorhanden");
        }

        // Sonderfall: zu löschender Student ist der Kopf
        if (*headStudent == student) {
            Student *toDelete = headStudent;
            headStudent = headStudent->getNext();
            delete toDelete;
            cout << "Student erfolgreich geloescht" << endl;
            return;
        }

        // Andernfalls durchsuche die Liste
        while (current->getNext() != nullptr) {
            if (*(current->getNext()) == student) {
                Student *toDelete = current->getNext();
                current->setNext(toDelete->getNext());
                delete toDelete;
                cout << "Student erfolgreich geloescht" << endl;
                return;
            }
            current = current->getNext();
        }
        // Student nicht gefunden
        throw invalid_argument("Student konnte nicht gefunden werden");
    }
};

int main() {
    // Verwaltung-Objekt erstellen, das als Einstiegspunkt dient
    Verwaltung *v1 = new Verwaltung();

    // Kurse aus den Anforderungen anlegen und zur Verwaltung hinzufügen
    Kurs *k1 = new Kurs("Programmieren");
    Kurs *k2 = new Kurs("Datenbanken");
    Kurs *k3 = new Kurs("Software Engineering");

    v1->addKurs(k1);
    v1->addKurs(k2);
    v1->addKurs(k3);

    // Option zum Laden von Daten aus Datei vor Programmstart
    string preChoice;
    cout << "Wollen Sie Daten laden (y/n)?" << endl;
    cin >> preChoice;

    // Wenn ja gewählt, Daten aus Datei laden
    if (preChoice == "y") {
        v1->ausDateiLesen("registrierung.txt");
    }

    // Steuerung des Programmflusses über while-Loop
    bool running = true;
    int choice;

    while (running) {
        // Hauptmenü ausgeben
        cout << "\n===== Kursverwaltungssystem =====\n";
        cout << "1. Fuer einen Kurs registrieren\n";
        cout << "2. Alle Kurse anzeigen\n";
        cout << "3. Alle nicht vollen Kurse anzeigen\n";
        cout << "4. Registrierung stornieren\n";
        cout << "5. Alle Studenten anzeigen\n";
        cout << "6. Programm beenden\n";
        cout << "7. Programm beenden und speichern\n";
        cout << "Bitte Auswahl treffen: ";
        cin >> choice;

        // Menüauswahl behandeln
        switch (choice) {
            case 1: {
                // Neue Registrierung eines Studenten
                Student *s = new Student();
                cout << "\n--- Fuer einen Kurs registrieren ---\n";
                cout << "Student anlegen:\n";
                cin >> *s;

                bool registrierungErfolgreich = false;

                // Wiederholung, bis Kurswahl erfolgreich ist
                while (!registrierungErfolgreich) {
                    cout << "\nVerfuegbare Kurse:\n";
                    Kurs *kurs = v1->printAllCourseNamesAndGiveSelection();

                    try {
                        kurs->addStudent(s); // Student zum Kurs hinzufügen
                        cout << "Student wurde erfolgreich fuer " << kurs->getName() << " registriert.\n";
                        v1->addStudent(s); // Student zur LinkedList hinzufügen
                        registrierungErfolgreich = true;
                    } catch (const invalid_argument &e) {
                        // Fehler bei Kursanmeldung behandeln (z.B. Kurs voll)
                        cout << "Fehler: " << e.what() << endl;
                        cout << "Bitte versuchen Sie es erneut.\n";
                    }
                }
                break;
            }
            case 2: {
                // Alle Kurse mit Teilnehmern anzeigen
                cout << "\n--- Alle Kurse ---\n";
                v1->printAllCourses();
                break;
            }
            case 3: {
                // Nur Kurse anzeigen, die noch freie Plätze haben
                cout << "\n--- Alle nicht vollen Kurse ---\n";
                v1->printAllNotFullCourses();
                break;
            }
            case 4: {
                // Registrierung eines Studenten stornieren
                cout << "\n--- Registrierung stornieren ---\n";
                Student *s = new Student();
                cin >> *s;

                try {
                    // Student in Verwaltung suchen und löschen
                    Student *foundS = v1->findStudent(*s);
                    v1->deleteStudent(*foundS);
                } catch (const invalid_argument &e) {
                    // Fehlerbehandlung, wenn Student nicht gefunden wurde
                    cout << "Fehler: " << e.what() << endl;
                }

                // Temporär angelegten Student wieder löschen
                delete s;
                break;
            }
            case 5: {
                // Alle registrierten Studenten anzeigen
                cout << "\n--- Alle Studenten ausgeben ---\n";
                v1->printAllStudents();
                break;
            }
            case 6: {
                // Programm beenden ohne zu speichern
                running = false;
                cout << "\nProgramm wird beendet.\n";
                break;
            }
            case 7: {
                // Programm beenden und aktuelle Daten in Datei speichern
                running = false;
                cout << "\nProgramm wird beendet.\n";
                v1->speichernInDatei("registrierung.txt");
                break;
            }
            default: {
                // Ungültige Eingabe abfangen
                cout << "\nUngueltige Eingabe! Bitte erneut versuchen.\n";
                break;
            }
        }
    }

    // Aufräumen und Speicher freigeben
    delete v1;
    return 0;
}
