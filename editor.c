#include <stdio.h>    // Ein- und Ausgabefunktionen wie printf, scanf
#include <stdlib.h>   // Speicherfunktionen wie malloc, free
#include <ctype.h>    // Für Zeichenprüfung: isprint()
#include <string.h>   // Für Zeichenkettenfunktionen wie strncpy, fgets

// Funktion: Wandelt ein Byte in eine 8-stellige Binär-Zeichenkette um
void byte_to_binary(unsigned char byte, char *bin_str) {
    for (int i = 7; i >= 0; i--) {
        // Prüfe jedes Bit von links nach rechts
        bin_str[7 - i] = (byte & (1 << i)) ? '1' : '0';
    }
    bin_str[8] = '\0';  // Zeichenkette beenden
}

// Funktion: Benutzer kann ein Byte im Speicher ändern
void bearbeiten(unsigned char *daten, int groesse) {
    int position;
    int neuer_wert;

    // Benutzer wählt Position (0 bis Dateigröße - 1)
    printf("\nWelche Position möchtest du ändern (0 bis %d)? ", groesse - 1);
    if (scanf("%d", &position) != 1 || position < 0 || position >= groesse) {
        printf("Ungültige Position.\n");
        while (getchar() != '\n'); // Eingabepuffer leeren
        return;
    }

    // Benutzer gibt neuen HEX-Wert ein (z. B. 41 für 'A')
    printf("Gib den neuen HEX-Wert ein (z.B. 41 für 'A'): 0x");
    if (scanf("%x", &neuer_wert) != 1 || neuer_wert < 0 || neuer_wert > 255) {
        printf("Ungültiger HEX-Wert.\n");
        while (getchar() != '\n');
        return;
    }

    // Wert wird im Speicher geändert
    daten[position] = (unsigned char)neuer_wert;
    printf("Byte an Position %d wurde geändert auf: 0x%02X\n", position, daten[position]);
}

// Hauptfunktion
int main(int argc, char *argv[]) {
    char dateiname[256];  // Hier wird der Dateiname gespeichert

    // Wenn der Dateiname beim Start mitgegeben wurde
    if (argc >= 2) {
        strncpy(dateiname, argv[1], sizeof(dateiname) - 1);
        dateiname[sizeof(dateiname) - 1] = '\0';
    } else {
        // Sonst fragt das Programm nach dem Dateinamen
        printf("Gib den Dateinamen ein: ");
        if (fgets(dateiname, sizeof(dateiname), stdin) == NULL) {
            printf("Fehler bei der Eingabe.\n");
            return 1;
        }
        dateiname[strcspn(dateiname, "\n")] = '\0';  // Zeilenumbruch entfernen
    }

    // Datei im Lese- und Schreibmodus öffnen
    FILE *datei = fopen(dateiname, "rb+");
    if (!datei) {
        printf("Datei konnte nicht geöffnet werden.\n");
        return 1;
    }

    // Dateigröße ermitteln
    // Schritt 1: Springe ans Ende der Datei
    fseek(datei, 0, SEEK_END);  // Das setzt den Dateizeiger ans Ende, um die Gesamtgröße zu messen

    // Schritt 2: Ermittle die aktuelle Position des Dateizeigers (am Ende)
    int dateigroesse = ftell(datei);    // Diese Position entspricht der Dateigröße in Bytes

    // Schritt 3: Setze den Dateizeiger wieder zurück an den Anfang
    rewind(datei);

    // Speicher reservieren für den Inhalt der Datei
    unsigned char *daten = malloc(dateigroesse);
    if (!daten) {
        printf("Fehler beim Speicher reservieren.\n");
        fclose(datei);
        return 1;
    }

    // Datei wird in den Speicher gelesen
    fread(daten, 1, dateigroesse, datei);

    // Inhalt der Datei anzeigen: Position, HEX, Binär, Zeichen
    printf("\nOffset  HEX  BIN       CHAR\n-----------------------------\n");
    for (int i = 0; i < dateigroesse; i++) {
        char bin_str[9];
        byte_to_binary(daten[i], bin_str);  // Binärdarstellung erzeugen
        char zeichen = isprint(daten[i]) ? daten[i] : '.';  // Nur druckbare Zeichen anzeigen
        printf("%06X  %02X   %s   %c\n", i, daten[i], bin_str, zeichen);
    }

    // Nach Bearbeitung fragen (mehrfach möglich)
    char antwort;
    do {
        printf("\nMöchtest du ein Byte bearbeiten? (j/n): ");
        scanf(" %c", &antwort);
        if (antwort == 'j' || antwort == 'J') {
            bearbeiten(daten, dateigroesse);  // Bearbeitungsfunktion aufrufen
        }
    } while (antwort == 'j' || antwort == 'J');

    // Datei wird mit den neuen Daten überschrieben
    rewind(datei);
    fwrite(daten, 1, dateigroesse, datei);
    printf("Datei wurde gespeichert.\n");

    // Speicher freigeben und Datei schließen
    free(daten);
    fclose(datei);
    return 0;
}
