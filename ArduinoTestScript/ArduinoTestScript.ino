/*
    This file ( ArduinoTestScript.ino ) is part of MinimalSerialMonitor.
    (C) 31.1.2021 Fidoriel

    MinimalSerialMonitor is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    MinimalSerialMonitor is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MinimalSerialMonitor.  If not, see <http://www.gnu.org/licenses/>.
*/

void setup()
{
	Serial.begin( 9600 );
}

void loop()
{
	if ( Serial.available() )
	{
		Serial.write( Serial.read() );
	}
}
