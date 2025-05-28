#include <stdio.h>   // für Ein- und Ausgabe
#include <stdlib.h>  // für Speicherverwaltung
#include <ctype.h>   // für isprint()

// Funktion: Wandelt ein Byte in einen 8-stelligen Binär-String
void byte_to_binary(unsigned char byte, char *bin_str) {
    for (int i = 7; i >= 0; i--) {
        bin_str[7 - i] = (byte & (1 << i)) ? '1' : '0';
    }
    bin_str[8] = '\0';  // Null-Terminierung
}

// Funktion: Bearbeitet ein Byte an einer bestimmten Position
void bearbeiten(unsigned char *daten, int groesse) {
    int pos, wert;
    printf("\nWelche Position möchtest du ändern (0-%d)? ", groesse - 1);
    scanf("%d", &pos);

    if (pos < 0 || pos >= groesse) {
        printf("Ungültige Position.\n");
        return;
    }

    printf("Neuer HEX-Wert für Position %d (z. B. 41 für 'A'): 0x", pos);
    scanf("%x", &wert);

    daten[pos] = (unsigned char)wert;
    printf("Geändert: Position %d = 0x%02X\n", pos, daten[pos]);
}

// HIER beginnt das Programm mit main()
int main(int argc, char *argv[]) {
    // Prüfen, ob Dateiname übergeben wurde
    if (argc < 2) {
        printf("Dateiname fehlt. Beispiel: ./editor test.txt\n");
        return 1;
    }

    // Datei im Lese-/Schreibmodus öffnen
    FILE *datei = fopen(argv[1], "rb+");
    if (!datei) {
        printf("Datei konnte nicht geöffnet werden.\n");
        return 1;
    }

    // Dateigröße ermitteln
    fseek(datei, 0, SEEK_END);
    int groesse = ftell(datei);
    rewind(datei);

    // Speicher für Dateipuffer reservieren
    unsigned char *daten = malloc(groesse);
    if (!daten) {
        printf("Speicherfehler.\n");
        fclose(datei);
        return 1;
    }

    // Datei in Speicher einlesen
    fread(daten, 1, groesse, datei);

    // Dateiinhalte tabellarisch ausgeben
    printf("\nOffset  HEX  BIN       CHAR\n-----------------------------\n");
    for (int i = 0; i < groesse; i++) {
        char bin[9];
        byte_to_binary(daten[i], bin);
        printf("%06X  %02X   %s   %c\n", i, daten[i], bin, isprint(daten[i]) ? daten[i] : '.');
    }

    // Benutzer nach Bearbeitung fragen
    char antwort;
    printf("\nByte bearbeiten? (j/n): ");
    scanf(" %c", &antwort);

    if (antwort == 'j' || antwort == 'J') {
        bearbeiten(daten, groesse);
        rewind(datei);
        fwrite(daten, 1, groesse, datei);
        printf("Datei gespeichert.\n");
    }

    // Aufräumen
    free(daten);
    fclose(datei);
    return 0;
}
