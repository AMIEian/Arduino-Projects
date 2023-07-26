//info : qoyyum@gmail.com
//perubahan: rutin adzan dan iqomah menjadi satu fungsi saja dengan input integer dari waktu sholat
// fix tahun kabisat untuk 100 dan 400 tahun
// tambahan seting jam kedip kedip
#include <EEPROM.h>
#include <EepromUtil.h>
#include <DMD.h>
#include <TimerOne.h>
#include "DS1302.h" // library RTC
//#include <Time.h>
#include "PrayerTimes.h"
double times[sizeof(TimeName) / sizeof(char*)];
DS1302 rtc(A5, A4, A3); // (CE,IO,SCLK)  pin terhubung ke RTC CE=reset IO=data
Time t;               // library RTC
#include <SystemFont5x7.h>
#include <angka6x13.h>
#include <font4x7.h>
#include <SPI.h>
char separator[] = {':', ' '};
const char *waktusholat[] = {"SUBUH", "DZUHUR", "'ASHAR", "MAGHRIB", "ISYA'"} ;
const char *szona[] = { "", "", "", "", "", "", "", "WIB ", "WITA", "WIT ",};
char *hari[] = {"SENIN", "SELASA", "RABU" , "KAMIS", "JUM'AT", "SABTU", "AHAD" };
int maxday[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
double lintang;
double bujur;
int zonawaktu;
int jedaiqomah;
unsigned long previousMillis = 0;
unsigned long currentMillis;
const int interval = 1000;

#define SETTING 2
#define UP 3
#define DOWN 4
#define ALARM 5

int mode = 0;
byte tjam;
byte tmenit;
byte tdetik;
byte ttanggal;
byte tbulan;
int ttahun;
int tdow;
int pos;
int subuhH;
int subuhM;
int terbitH;
int terbitM;
int dzuhurH;
int dzuhurM;
int asarH;
int asarM;
int magribH;
int magribM;
int isyaH;
int isyaM;
int imsakH;
int imsakM;
char textbuffer[10];
DMD dmd(1, 1); //untuk mengatur jumlah panel yang kamu pakai

void ScanDMD()
{
  dmd.scanDisplayBySPI();
}
//////////////////////SEEEEEEETTTTTTTUUUUUUUUP//////////////////////////////////////////////////////////////
void setup() {
  // put your setup code here, to run once:
  pinMode(SETTING, INPUT_PULLUP);
  pinMode(UP, INPUT_PULLUP);
  pinMode(DOWN, INPUT_PULLUP);
  pinMode(ALARM, OUTPUT);
  digitalWrite(ALARM, LOW); // alarm of dulu kan
  delay(100);
  digitalWrite(ALARM, HIGH); // alarm of dulu kan
  delay(1000);
  digitalWrite(ALARM, LOW); // alarm of dulu kan
  rtc.halt(false);
  rtc.writeProtect(false);
  // Setup Serial connection
  Serial.begin(9600);

  //setting JAM
  //  rtc.setDate(28, 9, 2017);
  // rtc.setTime(11, 35, 30);
  //  rtc.setDOW(4);

  //READ EEPROM DATA

  zonawaktu = EEPROM.read(1);
  jedaiqomah = EEPROM.read(2);
  //EepromUtil::eeprom_write_string(5,"-7.78");
  //EepromUtil::eeprom_write_string(15,"110.36");
  EepromUtil::eeprom_read_string(5, textbuffer, 10);
  lintang = atof(textbuffer);
  EepromUtil::eeprom_read_string(15, textbuffer, 10);
  bujur = atof(textbuffer);
  Serial.print("lintang = ");
  Serial.println(lintang);
  Serial.print("bujur = ");
  Serial.println(bujur);
  // fire UP DMD
  Timer1.initialize( 3000 );
  Timer1.attachInterrupt( ScanDMD );
  dmd.clearScreen( true );
  //dmd.setBrightness(255);
  //attachInterrupt(digitalPinToInterrupt(SETTING), interruptpress, FALLING);
}
////// LOOOOOOOOP ///////////////////////////////////////////////////////////////////////
void loop() {
  // put your main code here, to run repeatedly:
  
  int t1 = digitalRead(SETTING);  //tombol setting
  int t2 = digitalRead(UP);  // tombol Up
  int t3 = digitalRead(DOWN);  // tombol down

  if (t1 == LOW) {
    mode = mode + 1;
    Serial.print("mode = ");
    Serial.println(mode);
    dmd.clearScreen(true);
    delay(250);
    if (mode > 9) {
      mode = 0;
      dmd.clearScreen(true);
    }
  }

  getjadwal();
  if (mode == 1) {
    //setting jam
    //    dmd.clearScreen(true);
    dmd.selectFont(font4x7);
    dmd.drawString(8, 0, "JAM", 3 , GRAPHICS_NORMAL);
    int jam = tjam;
    int menit = tmenit;
    if (t2 == LOW) {
      jam = jam + 1;
      delay(100);
      if (jam > 23 ) {
        jam = 0;
      }
    }
    if (t3 == LOW) {
      jam = jam - 1;
      delay(100);
      if (jam < 0 ) {
        jam = 23;
      }
    }
    String xjam = Konversi(jam);
    int pjg = xjam.length() + 1;
    char sjam[pjg];
    xjam.toCharArray(sjam, pjg);
    String xmenit = Konversi(menit);
    char smenit[pjg];
    xmenit.toCharArray(smenit, pjg);
    dmd.selectFont(SystemFont5x7);
    //dmd.drawString(0, 8, sjam, pjg, GRAPHICS_NORMAL);
    //dmd.drawChar(12, 8, separator[currentMillis / 1000 % 2],  GRAPHICS_NORMAL);
    dmd.drawChar(12, 8, ':',  GRAPHICS_NORMAL);
    dmd.drawString(18, 8, smenit, pjg, GRAPHICS_NORMAL);
    if (separator[currentMillis / 250 % 2] == ' '){
      dmd.drawString(0, 8, sjam, pjg, GRAPHICS_NORMAL);
    }
    else dmd.drawString(0, 8, "  ", pjg, GRAPHICS_NORMAL);
    rtc.setTime(jam, tmenit, tdetik);
  }
  if (mode == 2) {
    //setting menit
    //    dmd.clearScreen(true);
    dmd.selectFont(font4x7);
    dmd.drawString(5, 0, "MENIT", 5 , GRAPHICS_NORMAL);
    int jam = tjam;
    int menit = tmenit;
    if (t2 == LOW) {
      menit = menit + 1;
      delay(100);
      if (menit > 59 ) {
        menit = 0;
      }
    }
    if (t3 == LOW) {
      menit = menit - 1;
      delay(100);
      if (menit < 0 ) {
        menit = 59;
      }
    }
    String xjam = Konversi(jam);
    int pjg = xjam.length() + 1;
    char sjam[pjg];
    xjam.toCharArray(sjam, pjg);
    String xmenit = Konversi(menit);
    //int pjg = xmenit.length() + 1;
    char smenit[pjg];
    xmenit.toCharArray(smenit, pjg);
    dmd.selectFont(SystemFont5x7);
    dmd.drawString(0, 8, sjam, pjg, GRAPHICS_NORMAL);
    //dmd.drawChar(12, 8, separator[currentMillis / 1000 % 2],  GRAPHICS_NORMAL);
    dmd.drawChar(12, 8, ':',  GRAPHICS_NORMAL);
    //dmd.drawString(18, 8, smenit, pjg, GRAPHICS_NORMAL);
    if (separator[currentMillis / 250 % 2] == ' '){
      dmd.drawString(18, 8, smenit, pjg, GRAPHICS_NORMAL);
    }
    else dmd.drawString(18, 8, "  ", pjg, GRAPHICS_NORMAL);
    rtc.setTime(tjam, menit, tdetik);
  }
  if (mode == 3) {
    //setting tahun
    //    dmd.clearScreen(true);
    dmd.selectFont(font4x7);
    dmd.drawString(3, 0, "TAHUN", 5 , GRAPHICS_NORMAL);
    int tahun = ttahun;
    int bulan = tbulan;
    if (bulan>12){
      bulan = 12;
    }
    int tanggal = ttanggal;
    if (tanggal > 31) {
      tanggal = 31;
    }
    if (t2 == LOW) {
      tahun = tahun + 1;
      delay(100);
      if (tahun > 2200 ) {
        tahun = 2200;
      }
    }
    if (t3 == LOW) {
      tahun = tahun - 1;
      delay(100);
      if (tahun < 2017 ) {
        tahun = 2017;
      }
    }
    String xtahun = Konversi(tahun);
    int pjg = xtahun.length() + 1;
    char stahun[pjg];
    xtahun.toCharArray(stahun, pjg);
    dmd.selectFont(SystemFont5x7);
    if (separator[currentMillis / 250 % 2] == ' '){
      dmd.drawString(0, 8, stahun, pjg, GRAPHICS_NORMAL);
    }
    else dmd.drawString(0, 8, "    ", pjg, GRAPHICS_NORMAL);
    rtc.setDate(tanggal, bulan, tahun);
  }
  if (mode == 4) {
    //setting bulan
    //    dmd.clearScreen(true);
    dmd.selectFont(font4x7);
    dmd.drawString(3, 0, "BULAN", 5 , GRAPHICS_NORMAL);
    int bulan = tbulan;
    int tanggal = ttanggal;
    if (t2 == LOW) {
      bulan = bulan + 1;
      delay(100);
      if (bulan > 12 ) {
        bulan = 1;
      }
    }
    if (t3 == LOW) {
      bulan = bulan - 1;
      delay(100);
      if (bulan < 1 ) {
        bulan = 12;
      }
    }
    String xbulan = Konversi(bulan);
    int pjg = xbulan.length() + 1;
    char sbulan[pjg];
    xbulan.toCharArray(sbulan, pjg);
    String xtanggal = Konversi(tanggal);
    char stanggal[pjg];
    xtanggal.toCharArray(stanggal, pjg);
    dmd.selectFont(SystemFont5x7);
    dmd.drawString(18, 8, stanggal, pjg, GRAPHICS_NORMAL);
    dmd.drawChar(12, 8, '/',  GRAPHICS_NORMAL);
    if (separator[currentMillis / 250 % 2] == ' '){
      dmd.drawString(0, 8, sbulan, pjg, GRAPHICS_NORMAL);
    }
    else dmd.drawString(0, 8, "  ", pjg, GRAPHICS_NORMAL);
    rtc.setDate(ttanggal, bulan, ttahun);
  }
  if (mode == 5) {
    //setting tanggal
    //    dmd.clearScreen(true);
    dmd.selectFont(font4x7);
    dmd.drawString(8, 0, "TGL", 3 , GRAPHICS_NORMAL);
    int bulan = tbulan;
    int tanggal = ttanggal;
    if (t2 == LOW) {
      tanggal = tanggal + 1;
      delay(100);
      if ((((ttahun % 4 == 0) && (ttahun % 100 != 0)) || (ttahun % 400 == 0)) && (tbulan ==2) ) {
        if (tanggal > maxday[tbulan] + 1 ) {
          tanggal = 1;
        }
      }
      else if (tanggal > maxday[tbulan] ) {
        tanggal = 1;
      }
    }
    if (t3 == LOW) {
      tanggal = tanggal - 1;
      delay(100);
      if (tanggal < 1 ) {
        if ((((ttahun % 4 == 0) && (ttahun % 100 != 0)) || (ttahun % 400 == 0)) && (tbulan ==2) ) {
          tanggal = maxday[tbulan] + 1;
        }
        else tanggal = maxday[tbulan];
      }
    }
    String xtanggal = Konversi(tanggal);
    int pjg = xtanggal.length() + 1;
    char stanggal[pjg];
    xtanggal.toCharArray(stanggal, pjg);
    String xbulan = Konversi(bulan);
    char sbulan[pjg];
    xbulan.toCharArray(sbulan, pjg);
    dmd.selectFont(SystemFont5x7);
    //dmd.drawString(0, 8, stanggal, pjg, GRAPHICS_NORMAL);
    dmd.drawString(0, 8, sbulan, pjg, GRAPHICS_NORMAL);
    dmd.drawChar(12, 8, '/',  GRAPHICS_NORMAL);
    if (separator[currentMillis / 250 % 2] == ' '){
      dmd.drawString(18, 8, stanggal, pjg, GRAPHICS_NORMAL);
    }
    else dmd.drawString(18, 8, "  ", pjg, GRAPHICS_NORMAL);
    rtc.setDate(tanggal, tbulan, ttahun);
    Serial.println(jumlahhari());
    Serial.println(((jumlahhari() - 4) % 7) + 1);
    rtc.setDOW(((jumlahhari() - 4) % 7) + 1);
  }
  if (mode == 6) {
    //setting jeda iqomah
    //    dmd.clearScreen(true);
    dmd.selectFont(font4x7);
    dmd.drawString(7, 0, "JEDA", 5 , GRAPHICS_NORMAL);
    int jeda = EEPROM.read(2);
    if (t2 == LOW) {
      jeda = jeda + 1;
      delay(100);
      if (jeda > 30 ) {
        jeda = 30;
      }
    }
    if (t3 == LOW) {
      jeda = jeda - 1;
      delay(100);
      if (jeda < 5 ) {
        jeda = 5;
      }
    }
    String xjeda = Konversi(jeda);
    int pjg = xjeda.length() + 1;
    char sjeda[pjg];
    xjeda.toCharArray(sjeda, pjg);
    dmd.selectFont(SystemFont5x7);
    if (separator[currentMillis / 250 % 2] == ' '){
      dmd.drawString(0, 8, sjeda, pjg, GRAPHICS_NORMAL);
    }
    else dmd.drawString(0, 8, "  ", pjg, GRAPHICS_NORMAL);
    dmd.drawString(14, 8, "mnt", pjg, GRAPHICS_NORMAL);
    jedaiqomah = jeda;
    EEPROM.write(2, jeda);
  }
  if (mode == 7) {
    //setting zona waktu
    //    dmd.clearScreen(true);
    dmd.selectFont(SystemFont5x7);
    dmd.drawString(1, 0, "ZONA:", 5 , GRAPHICS_NORMAL);
    //dmd.selectFont(font4x7);
    int zona = EEPROM.read(1);
    if (t2 == LOW) {
      zona = zona + 1;
      delay(100);
      if (zona > 9 ) {
        zona = 7;
      }
    }
    if (t3 == LOW) {
      zona = zona - 1;
      delay(100);
      if (zona < 7 ) {
        zona = 9;
      }
    }
    if (separator[currentMillis / 500 % 2] == ' '){
      dmd.drawString(1, 8, szona[zona], 4, GRAPHICS_NORMAL);
    }
    else dmd.drawString(1, 8, "    ", 4, GRAPHICS_NORMAL);
    zonawaktu = zona;
    EEPROM.write(1, zona);
  }
  if (mode == 8) {
    //setting LINTANG
    //    dmd.clearScreen(true);
    dmd.selectFont(font4x7);
    dmd.drawString(0, 0, "LINTANG", 7 , GRAPHICS_NORMAL);
    if (t2 == LOW) {
      lintang = lintang + 0.01;
      delay(5);
      if (lintang > 15 ) {
        lintang = 15;
      }
      if (lintang == 0  ) {  //pergantian minus biar tidak nyisa
        dmd.clearScreen( true );
      }
    }
    if (t3 == LOW) {
      lintang = lintang - 0.01;
      delay(5);
      if (lintang < -15 ) {
        lintang = -15;
      }
    }

    String xlintang = String(lintang);
    int pjg = xlintang.length() + 1;
    char slintang[pjg];
    xlintang.toCharArray(slintang, pjg);
    dmd.selectFont(font4x7);
    if (separator[currentMillis / 500 % 2] == ' '){
      dmd.drawString(0, 8, slintang, pjg, GRAPHICS_NORMAL);
    }
    else dmd.drawString(0, 8, "      ", pjg, GRAPHICS_NORMAL);
    EepromUtil::eeprom_write_string(5, slintang);
  }
  if (mode == 9) {
    //setting BUJUR
    //    dmd.clearScreen(true);
    dmd.selectFont(font4x7);
    dmd.drawString(1, 0, "BUJUR", 5 , GRAPHICS_NORMAL);
    if (t2 == LOW) {
      bujur = bujur + 0.01;
      delay(10);
      if (bujur > 145 ) {
        bujur = 145;
      }
    }
    if (t3 == LOW) {
      bujur = bujur - 0.01;
      delay(10);
      if (bujur < 90 ) {
        bujur = 90;
      }
    }
    String xbujur = String(bujur);
    int pjg = xbujur.length() + 1;
    char sbujur[pjg];
    xbujur.toCharArray(sbujur, pjg);
    dmd.selectFont(font4x7);
    if (separator[currentMillis / 500 % 2] == ' '){
      dmd.drawString(0, 8, sbujur, pjg, GRAPHICS_NORMAL);
    }
    else dmd.drawString(0, 8, "       ", pjg, GRAPHICS_NORMAL);
    EepromUtil::eeprom_write_string(15, sbujur);
  }

  if (mode == 0) {
    if (currentMillis - previousMillis < 15000) {
      normalMode();
    }
    else if (tjam == subuhH && tmenit == subuhM) {
      adzan(0);
    }
    else if (tdow == 5 && tjam == dzuhurH && tmenit == dzuhurM) {
      jumat();
    }
    else if (tjam == dzuhurH && tmenit == dzuhurM) {
      adzan(1);
    }
    else if (tjam == asarH && tmenit == asarM) {
      adzan(2);
    }
    else if (tjam == magribH && tmenit == magribM) {
      adzan(3);
    }
    else if (tjam == isyaH && tmenit == isyaM) {
      adzan(4);
    }
    else {
      runningtext();
    }
  }

}
///// END LOOOOOP///////////////////////////////////////////////////////////////////

unsigned long jumlahhari() {
  unsigned long d = ttanggal;
  unsigned long m = tbulan;
  unsigned long y = ttahun;
  unsigned long hb[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
  unsigned long ht = (y - 1970) * 365 - 1;
  unsigned long hs = hb[m - 1] + d;
  unsigned long kab = 0;
  unsigned long i;
  if (y % 4 == 0) {
    if (m > 2) {
      hs++;
    }
  }
  for (i = 1970; i < y; i++) {
    if (((i % 4 == 0) && (i % 100 != 0)) || (i % 400 == 0)) {
      kab++;
    }
  }
  return (ht + hs + kab);
}

/////////////////////untuk nambahi 0 di depan jika angka <10 //////////////////
String Konversi(int sInput) {
  if (sInput < 10)
  {
    return"0" + String(sInput);
  }
  else
  {
    return String(sInput);
  }
}

void normalMode() {

  dmd.selectFont(angka6x13);
  // Tampilan jam
  String xjam = Konversi(tjam);
  int pjg = xjam.length() + 1;
  char sjam[pjg];
  xjam.toCharArray(sjam, pjg);
  dmd.drawString(0, 0, sjam, pjg, GRAPHICS_NORMAL);

  //tampilan detikan
  dmd.selectFont(SystemFont5x7);
  dmd.drawChar(14, 5, separator[currentMillis / 500 % 2], GRAPHICS_NORMAL);

  // tampilan menit
  dmd.selectFont(angka6x13);
  String xmenit = Konversi(tmenit);
  int pjgmenit = xmenit.length() + 1;
  char smenit[pjgmenit];
  xmenit.toCharArray(smenit, pjgmenit);
  dmd.drawString(19, 0, smenit, pjgmenit, GRAPHICS_NORMAL);

  if (((ttahun % 100 == 0) && (ttahun % 400 != 0)) && (tbulan == 2) && (ttanggal == 29)){   //penyesuaian tahun kabisat 
    rtc.setDate(1, 3, ttahun);
  }
}

void runningtext() {
  //dmd.clearScreen(true);
  dmd.selectFont(SystemFont5x7);
  // Tampilan jam
  String xjam = Konversi(tjam);
  int pjg = xjam.length() + 1;
  char sjam[pjg];
  xjam.toCharArray(sjam, pjg);
  dmd.drawString(1, 0, sjam, pjg, GRAPHICS_NORMAL);

  //tampilan detikan
  dmd.drawChar(14, 0, separator[currentMillis / 500 % 2], GRAPHICS_NORMAL);

  // tampilan menit
  String xmenit = Konversi(tmenit);
  int pjgmenit = xmenit.length() + 1;
  char smenit[pjgmenit];
  xmenit.toCharArray(smenit, pjgmenit);
  dmd.drawString(20, 0, smenit, pjgmenit, GRAPHICS_NORMAL);
  dmd.selectFont(font4x7);
  String teks;
  teks =  String(hari[tdow - 1]) +
          " " + Konversi(ttanggal) + "-" + Konversi(tbulan) + "-" + String(ttahun) +
          " SUBUH " + Konversi(subuhH) + ":" + Konversi(subuhM) +
          " TERBIT " + Konversi(terbitH) + ":" + Konversi(terbitM) +
          " DZUHUR " + Konversi(dzuhurH) + ":" + Konversi(dzuhurM) +
          " 'ASHAR " + Konversi(asarH) + ":" + Konversi(asarM) +
          " MAGHRIB " + Konversi(magribH) + ":" + Konversi(magribM) +
          " ISYA' " + Konversi(isyaH) + ":" + Konversi(isyaM) +
          " IMSAK " + Konversi(imsakH) + ":" + Konversi(imsakM);
  int pjgteks = teks.length() + 1;
  char cteks[pjgteks];
  pos = pos - 1;
  delay(15);
  teks.toCharArray(cteks, pjgteks);
  dmd.drawString(15 + pos, 8, cteks, pjgteks, GRAPHICS_NORMAL);
  if (currentMillis - previousMillis > 49000) {
    currentMillis = millis();
    previousMillis = currentMillis;
    dmd.clearScreen(true);
    pos = 0;
  }
}

void getjadwal() {
  currentMillis = millis();
  t = rtc.getTime();
  ttanggal = t.date;
  tbulan  = t.mon;
  ttahun  = t.year;
  tjam    = t.hour;
  tmenit  = t.min;
  tdetik  = t.sec;
  tdow    = t.dow;
  set_calc_method(ISNA);
  set_asr_method(Shafii);
  set_high_lats_adjust_method(AngleBased);
  set_fajr_angle(20);
  set_isha_angle(18);
  get_prayer_times(ttahun, tbulan, ttanggal, lintang , bujur , zonawaktu, times); // jadwal sholat di masukkan ke variabel times
  get_float_time_parts(times[0] + 0.05, subuhH, subuhM);
  get_float_time_parts(times[1] , terbitH, terbitM);
  get_float_time_parts(times[2] + 0.05, dzuhurH, dzuhurM);
  get_float_time_parts(times[3] + 0.05, asarH, asarM);
  get_float_time_parts(times[4] + 0.05, magribH, magribM);
  get_float_time_parts(times[6] + 0.05, isyaH, isyaM);
  get_float_time_parts(times[0] - 0.117, imsakH, imsakM);
}

void persiapan(int number, String teks){
  
}

void adzan(int number) {
  dmd.clearScreen( true );
  dmd.selectFont(SystemFont5x7);
  dmd.drawString(2, 0, "ADZAN ", 5, 0); //koordinat tampilan
  dmd.selectFont(font4x7);
  dmd.drawString(0, 8, waktusholat[number], strlen(waktusholat[number]), 0); //koordinat tampilan
  digitalWrite(ALARM, HIGH);//alarm sholat
  delay(1000);
  digitalWrite(ALARM, LOW);//alarm sholat
  delay(1000);
  digitalWrite(ALARM, HIGH);//alarm sholat
  delay(1000);
  digitalWrite(ALARM, LOW);//alarm sholat
  delay(1000);
  digitalWrite(ALARM, HIGH);//alarm sholat
  delay(1000);
  digitalWrite(ALARM, LOW);//alarm sholat
  delay(1000);
  digitalWrite(ALARM, HIGH);//alarm sholat
  delay(1000);
  digitalWrite(ALARM, LOW);//alarm sholat
  delay(1000);
  delay(150000);                //waktu adzan 150000 = 2,5 menit

  // IQOMAH
  dmd.clearScreen( true );
  dmd.drawString(2, 0, "IQOMAH", 6, 0);
  int tampil;
  int detik = 60, menit = jedaiqomah - 1; // waktu jeda iqomah 9 menit 60 detik
  for (detik = 60; detik >= 0; detik--) {
    delay(1000);
    String iqomah = Konversi(menit) + ":" + Konversi(detik);
    int panjang_iqomah = iqomah.length() + 1;
    char waktu_iqomah[panjang_iqomah];
    iqomah.toCharArray(waktu_iqomah, panjang_iqomah);
    dmd.selectFont(SystemFont5x7);
    dmd.drawString(1, 9, waktu_iqomah, panjang_iqomah, 0);
    if (detik <= 0) {
      detik = 60;
      menit = menit - 1;
    }
    if (menit <= 0 && detik <= 1) {
      digitalWrite(ALARM, HIGH);//alarm sholat
      delay(1000);
      digitalWrite(ALARM, LOW);//alarm sholat
      delay(1000);
      digitalWrite(ALARM, HIGH);//alarm sholat
      delay(500);
      digitalWrite(ALARM, LOW);//alarm sholat
      delay(1000);
      for (tampil = 0; tampil < 600 ; tampil++) {  // waktu sholat 600 = 5 menit
        menit = 0;
        detik = 0;
        dmd.clearScreen( true );
        dmd.selectFont(font4x7);
        dmd.drawString(1, 0, "SHOLAT", 6, 0); //koordinat tampilan
        ///////////////////////
        //now = rtc.now();
        t = rtc.getTime();
        String xjam = Konversi(t.hour) + String(separator[millis() / 500 % 2]) + Konversi(t.min) ; //tampilan jam
        int pjg = xjam.length() + 1;
        char sjam[pjg];
        xjam.toCharArray(sjam, pjg);
        dmd.selectFont(SystemFont5x7);
        dmd.drawString(2, 9, sjam, pjg, 0);
        /////////////////////
        delay (500);
      }
    }
  }
}

void jumat() {
  digitalWrite(ALARM, HIGH);//alarm sholat
  delay(1000);
  digitalWrite(ALARM, LOW);//alarm sholat
  delay(1000);
  digitalWrite(ALARM, HIGH);//alarm sholat
  delay(1000);
  digitalWrite(ALARM, LOW);//alarm sholat
  delay(1000);
  int tampil;
  for (tampil = 0; tampil < 30100 ; tampil++) {  // waktu sholat jumat 1550 = 25 menit 
    dmd.clearScreen( true );
    dmd.selectFont(font4x7);
    dmd.drawString(1, 0, "JUM'AT", 6, 0); //koordinat tampilan
    ///////////////////////
    //now = rtc.now();
    t = rtc.getTime();
    String xjam = Konversi(t.hour) + String(separator[millis() / 500 % 2]) + Konversi(t.min) ; //tampilan jam
    int pjg = xjam.length() + 1;
    char sjam[pjg];
    xjam.toCharArray(sjam, pjg);
    dmd.selectFont(SystemFont5x7);
    dmd.drawString(2, 9, sjam, pjg, 0);
    /////////////////////
    delay (500);
  }
}
