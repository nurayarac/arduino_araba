int Echo1 = 3; // left_sensor echo
int Trig1 = 2; // left_sensor trig
int Echo2 = 5; // central_sensor echo
int Trig2 = 4; // central_sensor trig
int Echo3 = 7; // right_sensor echo
int Trig3 = 6; // right_sensor trig

int in1 = 8; // pin 6 arduino
int in2 = 11; // pin 5 arduino
int in3 = 12; // pin 4 arduino
int in4 = 13; // pin 3 arduino
int ENA = 9; // pin 7 arduino
int ENB = 10; // pin 2 arduino
int PWM = 100; // Hız ayarı 10 ile 255 arasında
int Left_Distance = 0, Right_Distance = 0, Middle_Distance = 0;
int LDR_Pin = A0; // LDR sensörünün bağlı olduğu pin
int LDR_Threshold = 200; // LDR eşiği, bu değeri çevrenize göre ayarlayabilirsiniz
bool passedFirstTunnel = false; // İlk tünelden geçti mi kontrolü

void _Forward() {
    digitalWrite(in1, HIGH); // İleri
    digitalWrite(in2, LOW);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
    analogWrite(ENA, PWM);
    analogWrite(ENB, PWM);
}

void _Back() {
    digitalWrite(in1, LOW); // Geri
    digitalWrite(in2, HIGH);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    analogWrite(ENA, PWM);
    analogWrite(ENB, PWM);
}

void _left() {
    digitalWrite(in1, LOW); // Sol
    digitalWrite(in2, HIGH);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
    analogWrite(ENA, PWM);
    analogWrite(ENB, PWM);
}

void _right() {
    digitalWrite(in1, HIGH); // Sağ
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    analogWrite(ENA, PWM);
    analogWrite(ENB, PWM);
}

void _Stop() {
    digitalWrite(in1, LOW); // Dur
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
    analogWrite(ENA, LOW);
    analogWrite(ENB, LOW);
}

// Ultrasonik mesafe ölçümü alt fonksiyonu
int Left_Distance_test() {
    digitalWrite(Trig1, LOW);
    delayMicroseconds(2);
    digitalWrite(Trig1, HIGH);
    delayMicroseconds(20);
    digitalWrite(Trig1, LOW);
    float Fdistance = pulseIn(Echo1, HIGH);
    delay(10);
    Fdistance = Fdistance / 29 / 2;
    return (int)Fdistance;
}

int Middle_Distance_test() {
    digitalWrite(Trig2, LOW);
    delayMicroseconds(2);
    digitalWrite(Trig2, HIGH);
    delayMicroseconds(20);
    digitalWrite(Trig2, LOW);
    float Fdistance = pulseIn(Echo2, HIGH);
    delay(10);
    Fdistance = Fdistance / 29 / 2;
    return (int)Fdistance;
}

int Right_Distance_test() {
    digitalWrite(Trig3, LOW);
    delayMicroseconds(2);
    digitalWrite(Trig3, HIGH);
    delayMicroseconds(20);
    digitalWrite(Trig3, LOW);
    float Fdistance = pulseIn(Echo3, HIGH);
    delay(10);
    Fdistance = Fdistance / 29 / 2;
    return (int)Fdistance;
}

void setup() {
    Serial.begin(9600);
    pinMode(Echo1, INPUT);
    pinMode(Trig1, OUTPUT);
    pinMode(Echo2, INPUT);
    pinMode(Trig2, OUTPUT);
    pinMode(Echo3, INPUT);
    pinMode(Trig3, OUTPUT);
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);
    pinMode(in3, OUTPUT);
    pinMode(in4, OUTPUT);
    pinMode(ENA, OUTPUT);
    pinMode(ENB, OUTPUT);

    _Stop();
}

void loop() {
    Left_Distance = Left_Distance_test();
    delay(10);
    Middle_Distance = Middle_Distance_test();
    delay(10);
    Right_Distance = Right_Distance_test();
    delay(10);

    int LDR_Value = analogRead(LDR_Pin);
    Serial.print("LDR Value: ");
    Serial.println(LDR_Value);

    if (!passedFirstTunnel) {
        // Eğer ışık seviyesi düşükse ve ilk tünelden geçilmediyse dur
        if (LDR_Value < LDR_Threshold) {
            _Stop();
            delay(5000); // 5 saniye bekle
            passedFirstTunnel = true; // İlk tünelden geçildi olarak işaretle
        }
    }

    Serial.print("Left_Distance: ");
    Serial.print(Left_Distance);
    Serial.print("cm\tMiddle_Distance: ");
    Serial.print(Middle_Distance);
    Serial.print("cm\tRight_Distance: ");
    Serial.println(Right_Distance);

   if ((Middle_Distance > 27 && Right_Distance > 27 && Left_Distance > 27) || (Middle_Distance > Right_Distance && Middle_Distance > Left_Distance)) {
        _Forward();
    }
    // Sağa dönüş koşulu
    else if (Left_Distance < 27 && Middle_Distance < 27 && Right_Distance > 27) {
        _Back();
        delay(100);
        _right();
        delay(130);
    }
    // Sola dönüş koşulu
    else if (Right_Distance < 27 && Middle_Distance < 27 && Left_Distance > 27) {
        _Back();
        delay(100);
        _left();
        delay(130);
    }
    // Engelden kaçınma manevrası
    else if (Middle_Distance <= 27) {
        _Stop();
        delay(100);
        // Sağ boşsa sağa dön
        if (Right_Distance > Left_Distance) {
            _Back();
            delay(100);
            _right();
            delay(130);
        }
        // Sol boşsa sola dön
        else {
            _Back();
            delay(100);
            _left();
            delay(130);
        }
    }
    else {
        _Forward();
    }
}
