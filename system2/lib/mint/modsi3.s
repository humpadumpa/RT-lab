/*
 *  modsi3.c
 *
 *  32-bit signed modulo: r0r1 %= r2r3
 *
 *  Calls the ROM version of divsi3, which leaves the absolute value of the
 *  remainder in r3r4.
 *
 *  The contents of this file are subject to the Mozilla Public License
 *  Version 1.0 (the "License"); you may not use this file except in
 *  compliance with the License. You may obtain a copy of the License at
 *  http://www.mozilla.org/MPL/
 *
 *  Software distributed under the License is distributed on an "AS IS"
 *  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 *  License for the specific language governing rights and limitations
 *  under the License.
 *
 *  The Original Code is Rcxlib code, released January 20, 1999.
 *
 *  The Initial Developer of the Original Code is Kekoa Proudfoot.
 *  Portions created by Kekoa Proudfoot are Copyright (C) 1999
 *  Kekoa Proudfoot. All Rights Reserved.
 *
 *  Contributor(s): Kekoa Proudfoot <kekoa@graphics.stanford.edu>
 */

        .NAME      MODSI3
        .GLOBAL    ___modsi3
        
___modsi3:
        push r4
        push r5
        push r6
        push r0
        mov.w r1,r6
        mov.w r0,r5
        mov.w r3,r4
        mov.w r2,r3
        jsr @@88
        mov.w r4,r1
        mov.w r3,r0
        /* negate remainder if numerator was less than zero\n\t" */
        btst #0x7,@r7
        beq skipnegate
        not.b r0l
        not.b r0h
        not.b r1l
        not.b r1h
        add.b #0x1,r1l
        addx #0x0,r1h
        addx #0x0,r0l
        addx #0x0,r0h
        skipnegate:
        adds #0x2,r7
        pop r6
        pop r5
        pop r4
        rts
        .END
        