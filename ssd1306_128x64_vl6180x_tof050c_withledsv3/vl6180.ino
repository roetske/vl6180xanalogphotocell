String f_measure()
{ 
   /*Poll measurement of ambient light data*/
  // float lux = VL6180X.alsPoLLMeasurement();
  // String str ="ALS: "+String(lux)+" lux";
  // Serial.println(str);
  /*Poll measurement of distance*/
  uint8_t range = VL6180X.rangePollMeasurement();
  gl_distance = range;
  delay(200);
  /*Get the judgment of the range value*/
  uint8_t status = VL6180X.getRangeResult();
 
  String str_measure = String(range) + " mm"; 
  if (status !=  VL6180X_NO_ERR)
   { str_measure = "Error";
    digitalWrite(redled,HIGH);
    digitalWrite(greenled,LOW);
        
   }
  //check error and go
  Serial.println("status");
  Serial.println(status);
  switch(status){
    case VL6180X_NO_ERR:
      Serial.println(str_measure);
      digitalWrite(redled,LOW);
      break;
    case VL6180X_EARLY_CONV_ERR:
      Serial.println("RANGE ERR: ECE check failed !");
      str_measure = "ECE check failed !";
      break;
    case VL6180X_MAX_CONV_ERR:
      Serial.println("RANGE ERR: System did not converge before the specified max!");
      str_measure ="Out of range";
      break;
    case VL6180X_IGNORE_ERR:
      Serial.println("RANGE ERR: Ignore threshold check failed !");
      str_measure ="Ignore threshold check failed !";
      break;
    case VL6180X_MAX_S_N_ERR:
      Serial.println("RANGE ERR: Measurement invalidated!");
       str_measure ="Measurement invalidated!";
       break;
    case VL6180X_RAW_Range_UNDERFLOW_ERR:
      Serial.println("RANGE ERR: RESULT_RANGE_RAW < 0!");
      str_measure ="RESULT_RANGE_RAW < 0!";
      break;
    case VL6180X_RAW_Range_OVERFLOW_ERR:
      Serial.println("RESULT_RANGE_RAW is out of range !");
      str_measure ="RESULT_RANGE_RAW is out of range !";
      break;
    case VL6180X_Range_UNDERFLOW_ERR:
      Serial.println("RANGE ERR: RESULT__RANGE_VAL < 0 !");
      str_measure ="RESULT__RANGE_VAL < 0 !";
      break;
    case VL6180X_Range_OVERFLOW_ERR:
      Serial.println("RANGE ERR: RESULT__RANGE_VAL is out of range !");
      str_measure = "RESULT__RANGE_VAL is out of range !";
      break;
    default:
      Serial.println("RANGE ERR: System err!");
      str_measure = "RANGE ERR: System err!";
      break;
  }
  return str_measure ;
}