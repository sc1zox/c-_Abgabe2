#include <fstream>
#include <iostream>
class Kurs;
using namespace std;

class Student {
private:
    string name;
    string vorname;
    int matnr;
    Kurs *kurs;
    Student *next;

public:
    Student() : matnr(0), kurs(nullptr), next(nullptr) {
    }

    void applyKurs(Kurs *kurs);

    Student *getNext() const {
        return next;
    }

    void setNext(Student *next) {
        this->next = next;
    }

    Kurs *getKurs() const {
        return kurs;
    }

    friend ostream &operator<<(ostream &os, const Student &student);

    friend istream &operator>>(istream &is, Student &student);

    friend bool operator==(const Student &, const Student &);

    friend class Verwaltung;
};

bool operator==(const Student &a, const Student &b) {
    return a.matnr == b.matnr && a.name == b.name && a.vorname == b.vorname;
}

ostream &operator<<(ostream &os, const Student &s) {
    os << "Name: " << s.name << ", Vorname: " << s.vorname << ", Matrikelnummer: " << s.matnr;
    return os;
}

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

void Student::applyKurs(Kurs *kurs) {
    if (this->kurs == nullptr) {
        this->kurs = kurs;
    } else {
        throw invalid_argument("Student ist schon in einem Kurs registriert");
    }
}

class Kurs {
private:
    static const int min = 3;
    static const int max = 8;
    Student *s[max];
    int teilnehmerzahl;
    string name;
    Kurs *next;

public:
    Kurs(const string &name): teilnehmerzahl(0), name(name), next(nullptr) {
        for (int i = 0; i < max; i++) s[i] = nullptr;
    }

    friend ostream &operator<<(ostream &os, const Kurs &kurs);

    friend istream &operator>>(istream &is, Kurs &kurs);

    friend bool operator==(const Kurs &, const Kurs &);

    string getName() const {
        return name;
    }

    int getTeilnehmerzahl() const {
        return teilnehmerzahl;
    }

    void setName(const string &name) {
        this->name = name;
    }

    void setTeilnehmerzahl(int teilnehmerzahl) {
        this->teilnehmerzahl = teilnehmerzahl;
    }

    void addStudent(Student *student) {
        if (teilnehmerzahl < max) {
            try {
                student->applyKurs(this);
                this->s[teilnehmerzahl] = student;
                teilnehmerzahl++;
            } catch (invalid_argument &e) {
                throw;
            }
        } else {
            throw invalid_argument("Kurs ist voll");
        }
    }

    void removeStudent(const Student *student) {
        if (student == nullptr) {
            throw invalid_argument("Student ist null");
        }
        if (student->getKurs()->getName() != name) {
            throw invalid_argument("Student ist nicht im Kurs registriert");
        }

        int index = -1;
        // Finde den Index des zu löschenden Studenten
        for (int i = 0; i < teilnehmerzahl; i++) {
            if (s[i] == student) {
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

            teilnehmerzahl--;
        } else {
            throw invalid_argument("Student nicht im Kurs-Array gefunden");
        }
    }

    void printStudents() const {
        for (int i = 0; i < teilnehmerzahl; i++) {
            if (s[i] != nullptr) {
                cout << "Student " << i + 1 << ": " << *s[i] << endl;
            } else {
                cout << "Student " << i + 1 << ": <nicht verfügbar>" << endl;
            }
        }
    }

    int getFreiePlaetze() const {
        return max - teilnehmerzahl;
    }

    Kurs *getNext() const {
        return next;
    }

    void setNext(Kurs *next) {
        this->next = next;
    }
};

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

class Verwaltung {
private:
    Kurs *headKurs;
    Student *headStudent;

public:
    Verwaltung() : headKurs(nullptr), headStudent(nullptr) {
    }

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


    void speichernInDatei(const string &dateiname) const {
        ofstream datei(dateiname);

        if (!datei.is_open()) {
            cerr << "Fehler beim Oeffnen der Datei zum Schreiben!" << endl;
            return;
        }

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

    void ausDateiLesen(const string &dateiname) {
        ifstream datei;
        datei.open(dateiname, ios::in);

        if (!datei.is_open()) {
            cerr << "Fehler beim Öffnen der Datei zum Lesen!" << endl;
            return;
        }

        string zeile;

        string studentName;
        string studentVorname;
        string studentKurs;
        int studentMatnr = 0;

        while (!datei.eof()) {
            getline(datei, zeile);

            if (zeile.empty()) continue;

            int pos = zeile.find(':');
            string label = zeile.substr(0, pos);
            string wert = zeile.substr(pos + 2); // +2 um das Leerzeichen nach dem : zu überspringen


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

                Student *neuerStudent = new Student();

                neuerStudent->name = studentName;
                neuerStudent->vorname = studentVorname;
                neuerStudent->matnr = studentMatnr;

                addStudent(neuerStudent);

                try {
                    Kurs *k = findKurs(studentKurs);
                    k->addStudent(neuerStudent);
                } catch (const invalid_argument &e) {
                    cerr << "Fehler: " << e.what() << endl;
                }
            }
        }

        datei.close();
        cout << "Daten wurden aus " << dateiname << " geladen." << endl;
    }


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

    Kurs *getHeadKurs() const {
        return headKurs;
    }

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

    Kurs *printAllCourseNamesAndGiveSelection() {
        Kurs *current = headKurs;

        if (headKurs == nullptr) {
            throw invalid_argument("Keine Kurse vorhanden");
        }

        int i = 1;
        while (current != nullptr) {
            cout << i++ << ": " << current->getName() << endl;
            current = current->getNext();
        }


        int auswahl;
        cout << "Bitte Kurs auswaehlen (1-" << (i - 1) << "): ";
        cin >> auswahl;

        while (auswahl < 1 || auswahl > (i - 1)) {
            cout << "Ungueltige Auswahl! Bitte erneut versuchen (1-" << (i - 1) << "): ";
            cin >> auswahl;
        }


        current = headKurs;
        for (int j = 1; j < auswahl; j++) {
            current = current->getNext();
        }


        return current;
    }

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

    void deleteStudent(Student &student) {
        Student *current = headStudent;

        Kurs *deleteStudentInKurs = student.kurs;
        deleteStudentInKurs->removeStudent(&student);

        if (headStudent == nullptr) {
            throw invalid_argument("Keine Studenten vorhanden");
        }

        if (*headStudent == student) {
            Student *toDelete = headStudent;
            headStudent = headStudent->getNext();
            delete toDelete;
            cout << "Student erfolgreich geloescht" << endl;
            return;
        }


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

        throw invalid_argument("Student konnte nicht gefunden werden");
    }
};

int main() {
    Verwaltung *v1 = new Verwaltung();

    Kurs *k1 = new Kurs("Programmieren");
    Kurs *k2 = new Kurs("Datenbanken");
    Kurs *k3 = new Kurs("Software Engineering");

    v1->addKurs(k1);
    v1->addKurs(k2);
    v1->addKurs(k3);

    string preChoice;
    cout << "Wollen Sie Daten laden (y/n)?" << endl;
    cin >> preChoice;


    if (preChoice == "y") {
        v1->ausDateiLesen("registrierung.txt");
    }

    bool running = true;
    int choice;

    while (running) {
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

        switch (choice) {
            case 1: {
                Student *s = new Student();
                cout << "\n--- Fuer einen Kurs registrieren ---\n";
                cout << "Student anlegen:\n";
                cin >> *s;

                bool registrierungErfolgreich = false;

                while (!registrierungErfolgreich) {
                    cout << "\nVerfuegbare Kurse:\n";
                    Kurs *kurs = v1->printAllCourseNamesAndGiveSelection();

                    try {
                        kurs->addStudent(s);
                        cout << "Student wurde erfolgreich fuer " << kurs->getName() << " registriert.\n";
                        v1->addStudent(s);
                        registrierungErfolgreich = true;
                    } catch (const invalid_argument &e) { //https://en.cppreference.com/w/cpp/error/invalid_argument
                        cout << "Fehler: " << e.what() << endl;
                        cout << "Bitte versuchen Sie es erneut.\n";
                    }
                }

                break;
            }
            case 2: {
                cout << "\n--- Alle Kurse ---\n";
                v1->printAllCourses();
                break;
            }
            case 3: {
                cout << "\n--- Alle nicht vollen Kurse ---\n";
                v1->printAllNotFullCourses();
                break;
            }
            case 4: {
                cout << "\n--- Registrierung stornieren ---\n";
                Student *s = new Student();
                cin >> *s;
                try {
                    Student *foundS = v1->findStudent(*s);
                    v1->deleteStudent(*foundS);
                } catch (const invalid_argument &e) {
                    cout << "Fehler: " << e.what() << endl;
                }
                delete s;
                break;
            }
            case 5: {
                cout << "\n--- Alle Studenten ausgeben ---\n";
                v1->printAllStudents();
                break;
            }
            case 6: {
                running = false;
                cout << "\nProgramm wird beendet.\n";
                break;
            }
            case 7: {
                running = false;
                cout << "\nProgramm wird beendet.\n";
                v1->speichernInDatei("registrierung.txt");
                break;
            }
            default: {
                cout << "\nUngueltige Eingabe! Bitte erneut versuchen.\n";
                break;
            }
        }
    }

    delete v1;
    return 0;
}
