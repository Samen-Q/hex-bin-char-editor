#include <stdio.h>   // für printf, scanf, FILE-Funktionen
#include <stdlib.h>  // für Speicherverwaltung (malloc, free)
#include <ctype.h>   // für isprint() – um druckbare Zeichen zu erkennen

// Funktion: Wandelt ein Byte in eine 8-stellige Binärdarstellung als String
// Beispiel: 0x41 (65 dezimal) → "01000001"
void byte_to_binary(unsigned char byte, char *bin_str) {
    for (int i = 7; i >= 0; i--) {
        // prüft jedes Bit von rechts nach links und setzt '1' oder '0' im String
        bin_str[7 - i] = (byte & (1 << i)) ? '1' : '0';
    }
    bin_str[8] = '\0';  // String korrekt abschließen
}

// Funktion: Erlaubt dem Benutzer, ein einzelnes Byte im Speicher zu bearbeiten
void bearbeiten(unsigned char *daten, int groesse) {
    int pos, wert;

    // Position des zu ändernden Bytes abfragen
    printf("\nWelche Position möchtest du ändern (0-%d)? ", groesse - 1);
    scanf("%d", &pos);

    // Prüfen, ob die Eingabe im gültigen Bereich liegt
    if (pos < 0 || pos >= groesse) {
        printf("Ungültige Position.\n");
        return;
    }

    // Benutzer gibt neuen Byte-Wert im HEX-Format ein (z. B. 0x41 für 'A')
    printf("Neuer HEX-Wert für Position %d (z.B. 41 für 'A'): 0x", pos);
    scanf("%x", &wert);

    // Neuer Wert wird im Speicher (im Array) gesetzt
    daten[pos] = (unsigned char)wert;

    // Bestätigung anzeigen
    printf("Geändert: Position %d = 0x%02X\n", pos, daten[pos]);
}

// Hauptprogramm – Einstiegspunkt in C
int main(int argc, char *argv[]) {
    // Prüfen, ob ein Dateiname übergeben wurde
    if (argc < 2) {
        printf("./editor test.txt\n");
        return 1;
    }

    // Datei im Lese-/Schreibmodus öffnen 
    FILE *datei = fopen(argv[1], "read_binary_write");
    if (!datei) {
        printf("Datei konnte nicht geöffnet werden.\n");
        return 1;
    }

    // Dateigröße ermitteln: ans Ende springen, Position abfragen, zurück zum Anfang
    fseek(datei, 0, SEEK_END);
    int groesse = ftell(datei);
    rewind(datei);

    // Dynamischen Speicher reservieren, um Datei in ein Array zu laden
    unsigned char *daten = malloc(groesse);
    if (!daten) {
        printf("Speicherfehler.\n");
        fclose(datei);
        return 1;
    }

    // Datei in das Array einlesen
    fread(daten, 1, groesse, datei);

    // Dateiinhalt in tabellarischer Form anzeigen: Offset, HEX, Binär, Zeichen
    printf("\nOffset  HEX  BIN       CHAR\n-----------------------------\n");
    for (int i = 0; i < groesse; i++) {
        char bin[9];  // 8 Zeichen + Nullterminierung
        byte_to_binary(daten[i], bin);  // Binärdarstellung erzeugen

        // Offset = Speicherposition in HEX
        // HEX-Wert = zweistellig
        // Binärwert = 8 Zeichen
        // Zeichen = falls druckbar, sonst '.'
        printf("%06X  %02X   %s   %c\n", i, daten[i], bin, isprint(daten[i]) ? daten[i] : '.');
    }

    // Benutzer fragen, ob er ein Byte bearbeiten will
    char antwort;
    printf("\nByte bearbeiten? (j/n): ");
    scanf(" %c", &antwort);

    if (antwort == 'j' || antwort == 'J') {
        bearbeiten(daten, groesse);          // Byte bearbeiten
        rewind(datei);                       // Zurück zum Dateianfang
        fwrite(daten, 1, groesse, datei);    // Änderungen speichern
        printf("Datei gespeichert.\n");
    }

    // Speicher und Datei wieder freigeben
    free(daten);
    fclose(datei);
    return 0;
}
