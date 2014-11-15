
void setup (void)
{
  Serial.begin (57600);   // debugging

}  // end of setup


// main loop - wait for flag set in interrupt routine
void loop (void)
{
    Serial.println ("This is a test");
    delay(1000);
}  // end of loop
