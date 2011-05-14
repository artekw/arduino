const int led[] = {11,12,13};

const int delayTime = 20;
#define RG 0
#define GB 1
#define BR 2

void setColour(const int * led, const int * colour)
{
    for(int i = 0; i < 3; i ++)
    {
        analogWrite(led[i],colour[i]);
    }
}

void cycle(int which, const int * led)
{
    int a;
    int b;
    switch(which)
    {
        case RG:
            a = led[0];
            b = led[1];
        break;
        case GB:
            a = led[1];
            b = led[2];
        break;
        default:
            a = led[2];
            b = led[0];
    }

    for(int i = 0; i <= 255; i ++)
    {
        analogWrite(b, i);
        analogWrite(a, 255 - i);
        delay(delayTime);
    }
}

void setup()
{
    for(int i = 0; i < 3; i++)
    {
        pinMode(led[i],OUTPUT);
    }
}

void loop()
{
    cycle(RG, led);
    cycle(GB, led);
    cycle(BR, led);
}
