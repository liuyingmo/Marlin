/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "../../inc/MarlinConfig.h"

#if ENABLED(PIDTEMPBED)

#include "../gcode.h"
#include "../../module/temperature.h"

#if BED_ZONE_HEAT
#include "../../module/zone_heat.h"
#endif
/**
 * M304 - Set and/or Report the current Bed PID values
 *
 *  P<pval> - Set the P value
 *  I<ival> - Set the I value
 *  D<dval> - Set the D value
 */
void GcodeSuite::M304() {
#if BED_ZONE_HEAT
  if (!parser.seen("PID")) return M304_report();
  if (parser.seen('P')) thermalManager.temp_bed.pid.Kp = parser.value_float();
  if (parser.seen('I')) thermalManager.temp_bed.pid.Ki = scalePID_i(parser.value_float());
  if (parser.seen('D')) thermalManager.temp_bed.pid.Kd = scalePID_d(parser.value_float());

  if (!parser.seen('P'))return;
  g_m304.p = parser.value_float();
  if (!parser.seen('I'))return;
  g_m304.i = parser.value_float();
  if (!parser.seen('D'))return;
  g_m304.d = parser.value_float();
  g_m304.z =parser.seen('Z')?parser.value_int():-1;
   
  
#else
  if (!parser.seen("PID")) return ();
  if (parser.seen('P')) thermalManager.temp_bed.pid.Kp = parser.value_float();
  if (parser.seen('I')) thermalManager.temp_bed.pid.Ki = scalePID_i(parser.value_float());
  if (parser.seen('D')) thermalManager.temp_bed.pid.Kd = scalePID_d(parser.value_float());
#endif
}

void GcodeSuite::M304_report(const bool forReplay/*=true*/) {
  report_heading_etc(forReplay, F(STR_BED_PID));
#if BED_ZONE_HEAT
#define REPROT_PID(XXX) SERIAL_ECHOLNPGM("  M304 Z",XXX," P", par_config.pid##XXX[0], " I", par_config.pid##XXX[1], " D", par_config.pid##XXX[2]);
  REPROT_PID(1); REPROT_PID(2); REPROT_PID(3); REPROT_PID(4);
  REPROT_PID(5); REPROT_PID(6); REPROT_PID(7); REPROT_PID(8);
  REPROT_PID(9);
#else
  SERIAL_ECHOLNPGM(
      "  M304 P", thermalManager.temp_bed.pid.Kp
    , " I", unscalePID_i(thermalManager.temp_bed.pid.Ki)
    , " D", unscalePID_d(thermalManager.temp_bed.pid.Kd)
  );
#endif
}

#endif // PIDTEMPBED
