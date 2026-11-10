#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

// ===== À remplir avec tes infos =====
const char* WIFI_NOM = "NOM_DE_TA_BOX";
const char* WIFI_MDP = "MOT_DE_PASSE_WIFI";
String TOKEN   = "TON_TOKEN";
String CHAT_ID = "TON_CHAT_ID";
// ====================================

const int CAPTEUR = 27;              // broche OUT du capteur PIR
const unsigned long PAUSE = 30000;   // 30 secondes entre deux alertes
int etatAvant = LOW;
unsigned long derniereAlerte = 0;

// Traduit les accents, emojis et espaces pour l'adresse web
String encoder(String texte) {
  String resultat = "";
  for (int i = 0; i < texte.length(); i++) {
    char c = texte[i];
    if (isalnum(c)) {
      resultat += c;
    } else {
      char code[4];
      sprintf(code, "%%%02X", (unsigned char)c);
      resultat += code;
    }
  }
  return resultat;
}

// Envoie un message sur Telegram
void envoyerMessage(String texte) {
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;

  String url = "https://api.telegram.org/bot" + TOKEN +
               "/sendMessage?chat_id=" + CHAT_ID + "&text=" + encoder(texte);

  http.begin(client, url);
  int code = http.GET();
  Serial.print("Reponse de Telegram : ");
  Serial.println(code);
  http.end();
}

void setup() {
  Serial.begin(115200);
  pinMode(CAPTEUR, INPUT);

  WiFi.begin(WIFI_NOM, WIFI_MDP);
  Serial.print("Connexion au Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connecte !");

  Serial.println("Le capteur chauffe, attends 30 secondes...");
  delay(30000);
  envoyerMessage("✅ Surveillance de ta chambre activée");
  Serial.println("Surveillance active !");
}

// Surveille en boucle
void loop() {
  int etat = digitalRead(CAPTEUR);

  if (etat == HIGH && etatAvant == LOW && millis() - derniereAlerte > PAUSE) {
    Serial.println("Quelqu'un est entre !");
    envoyerMessage("🚨 Quelqu'un est entré dans ta chambre !");
    derniereAlerte = millis();
  }

  etatAvant = etat;
}
