
/*
 * File: msgbox.c
 * Author: Justin Zaun
 * Project: GGZ GTK Client
 *
 * This is the main program body for the GGZ client
 *
 * Copyright (C) 2000, 2001 Justin Zaun.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#include <gtk/gtk.h>
#include <config.h>
#include "msgbox.h"

static MBReturn mb_status;

void CloseTheApp (GtkWidget *window, gpointer data)
{
	gtk_main_quit();
}


void DialogOKClicked (GtkButton *button, gpointer data)
{
	mb_status = MSGBOX_OK;
	gtk_widget_destroy (GTK_WIDGET (data));
}

void DialogCancelClicked (GtkButton *button, gpointer data)
{
	mb_status = MSGBOX_CANCEL;
	gtk_widget_destroy (GTK_WIDGET (data));
}

void DialogYesClicked (GtkButton *button, gpointer data)
{
	mb_status = MSGBOX_YES;
	gtk_widget_destroy (GTK_WIDGET (data));
}

void DialogNoClicked (GtkButton *button, gpointer data)
{
	mb_status = MSGBOX_NO;
	gtk_widget_destroy (GTK_WIDGET (data));
}

GtkWidget *AddWidget (GtkWidget *widget, GtkWidget *packingbox)
{
	gtk_box_pack_start (GTK_BOX (packingbox), widget, TRUE, TRUE, 2);
	return widget;
}


/* XPM */
/* XPM */
static char * stop[] = {
"32 32 219 2",
"  	c None",
". 	c #000000",
"+ 	c #C74E44",
"@ 	c #C64D43",
"# 	c #C54C43",
"$ 	c #C44B42",
"% 	c #C34A41",
"& 	c #C24940",
"* 	c #C1483E",
"= 	c #C0473D",
"- 	c #BF463C",
"; 	c #BD453B",
"> 	c #BD443A",
", 	c #BC4339",
"' 	c #BB4239",
") 	c #C84F46",
"! 	c #FFB5AF",
"~ 	c #FCEDEB",
"{ 	c #FBECEA",
"] 	c #FAEBE9",
"^ 	c #F9EAE8",
"/ 	c #F8E9E7",
"( 	c #F6E7E5",
"_ 	c #F5E6E4",
": 	c #F4E4E2",
"< 	c #F3E3E1",
"[ 	c #F2E2E0",
"} 	c #CBBCB6",
"| 	c #886158",
"1 	c #5E2A21",
"2 	c #C95047",
"3 	c #FFB6B0",
"4 	c #FFFFFF",
"5 	c #E79D95",
"6 	c #E69C94",
"7 	c #E59B93",
"8 	c #E49A92",
"9 	c #E29890",
"0 	c #E0968F",
"a 	c #DF958D",
"b 	c #DD938C",
"c 	c #DC928A",
"d 	c #C5736A",
"e 	c #B14F45",
"f 	c #B04E44",
"g 	c #7B0301",
"h 	c #400200",
"i 	c #CA5147",
"j 	c #FFB7B2",
"k 	c #D88E87",
"l 	c #E07B75",
"m 	c #D67A75",
"n 	c #D57974",
"o 	c #D47973",
"p 	c #D27771",
"q 	c #CF746F",
"r 	c #CD736D",
"s 	c #CC726B",
"t 	c #CB716A",
"u 	c #C97069",
"v 	c #BB6158",
"w 	c #9D0401",
"x 	c #7D0300",
"y 	c #3F0200",
"z 	c #FFB8B2",
"A 	c #DD9088",
"B 	c #D17169",
"C 	c #DB706A",
"D 	c #DA726B",
"E 	c #D9716A",
"F 	c #D76F69",
"G 	c #D46C65",
"H 	c #D26860",
"I 	c #D1655A",
"J 	c #CD5F55",
"K 	c #CA574C",
"L 	c #C54B41",
"M 	c #B90902",
"N 	c #B90702",
"O 	c #9D0400",
"P 	c #7C0300",
"Q 	c #DD9089",
"R 	c #D27269",
"S 	c #DC746D",
"T 	c #DC6F6A",
"U 	c #DF7671",
"V 	c #DE7670",
"W 	c #DB756F",
"X 	c #D8726B",
"Y 	c #D46E67",
"Z 	c #D16961",
"` 	c #CF635C",
" .	c #CC5D54",
"..	c #C85548",
"+.	c #C3443A",
"@.	c #BD2717",
"#.	c #B90701",
"$.	c #B80701",
"%.	c #9C0400",
"&.	c #DF766F",
"*.	c #DE716B",
"=.	c #E27C76",
"-.	c #DF7973",
";.	c #DB7670",
">.	c #D8726C",
",.	c #D46D66",
"'.	c #D1685F",
").	c #CE6158",
"!.	c #CA5A4F",
"~.	c #F7F7F6",
"{.	c #BD2719",
"].	c #610200",
"^.	c #250100",
"/.	c #E99F97",
"(.	c #E27D76",
"_.	c #DC746E",
":.	c #DF726C",
"<.	c #E3807A",
"[.	c #DF7A76",
"}.	c #DA7670",
"|.	c #D5706A",
"1.	c #D16A63",
"2.	c #CE635C",
"3.	c #F8F8F7",
"4.	c #F6F6F5",
"5.	c #B80601",
"6.	c #7D0200",
"7.	c #580200",
"8.	c #3F0100",
"9.	c #DA706A",
"0.	c #E2807B",
"a.	c #DC7973",
"b.	c #D7736D",
"c.	c #D1655F",
"d.	c #FAFAF9",
"e.	c #F5F5F4",
"f.	c #8B0300",
"g.	c #660200",
"h.	c #DA726A",
"i.	c #DB6F69",
"j.	c #DE7B75",
"k.	c #D36962",
"l.	c #FDFDFC",
"m.	c #900300",
"n.	c #6B0200",
"o.	c #D9706A",
"p.	c #DE7671",
"q.	c #E27B76",
"r.	c #D76C66",
"s.	c #FCFCFB",
"t.	c #F9F9F8",
"u.	c #C44B41",
"v.	c #6C0200",
"w.	c #D76F68",
"x.	c #DA756F",
"y.	c #E07974",
"z.	c #E3817B",
"A.	c #FFFFFE",
"B.	c #FBFBFA",
"C.	c #F8F8F8",
"D.	c #C7574C",
"E.	c #C44A40",
"F.	c #D56C65",
"G.	c #E2817C",
"H.	c #CA5E52",
"I.	c #C7554B",
"J.	c #C4463C",
"K.	c #D1766F",
"L.	c #D26861",
"M.	c #D46E66",
"N.	c #DE7973",
"O.	c #DE7A75",
"P.	c #FEFEFD",
"Q.	c #CA6058",
"R.	c #C85A50",
"S.	c #C65146",
"T.	c #C13F36",
"U.	c #D1655B",
"V.	c #D46C67",
"W.	c #D57069",
"X.	c #D7726D",
"Y.	c #C7564B",
"Z.	c #C54B40",
"`.	c #CE5F54",
" +	c #CF635A",
".+	c #D16860",
"++	c #D16963",
"@+	c #C24036",
"#+	c #CB5D54",
"$+	c #CF6963",
"%+	c #CA6059",
"&+	c #F4F4F3",
"*+	c #DB9189",
"=+	c #C95648",
"-+	c #CA5A4D",
";+	c #C95D52",
">+	c #8F0300",
",+	c #DCCCC7",
"'+	c #C4726A",
")+	c #C4433A",
"!+	c #C65246",
"~+	c #C54C40",
"{+	c #C24237",
"]+	c #9A7268",
"^+	c #C44A3F",
"/+	c #C3473C",
"(+	c #C14238",
"_+	c #930300",
":+	c #9E3C34",
"<+	c #980401",
"[+	c #950300",
"}+	c #830300",
"|+	c #630200",
"1+	c #940300",
"2+	c #620200",
"3+	c #B90601",
"4+	c #7A0200",
"5+	c #590200",
"6+	c #670200",
"                                                                ",
"                                                                ",
"                    . . . . . . . . . . . . .                   ",
"                  . + @ # $ % & * = - ; > , ' .                 ",
"                . ) ! ~ { ] ^ / ( _ : < [ } | 1 .               ",
"              . 2 3 4 5 6 7 8 9 0 a b c d e f g h .             ",
"            . i j 4 k l m n o p p q r s t u v w x y .           ",
"          . i z 4 A B C D E F G H I J K L L M N O P y .         ",
"        . 2 j 4 Q R S T U V W X Y Z `  ...+.@.#.$.%.P y .       ",
"      . ) 3 4 A R &.*.4 *.=.-.;.>.,.'.).!.~.{.@.$.$.%.].^..     ",
"    . + ! 4 /.(._.*.4 4 4 :.<.[.}.|.1.2.3.~.4.@.$.$.5.6.7.8..   ",
"    . @ ~ 5 l 9.T 4 4 4 4 4 *.0.a.b.c.d.3.4.4.e.$.$.5.f.g.8..   ",
"    . # { 6 m h.U *.4 4 4 4 4 i.j.k.l.d.3.4.e.@.$.5.5.m.n.8..   ",
"    . $ ] 7 n o.p.q.:.4 4 4 4 4 r.4 s.t.~.4.u.@.$.5.5.m.v.8..   ",
"    . % ^ 8 o w.x.y.z.*.4 4 4 4 4 A.B.C.~.D.E.@.5.5.5.m.v.8..   ",
"    . & / 9 p F.X ;.[.G.i.4 4 4 4 l.d.3.H.I.J.@.5.5.5.m.v.8..   ",
"    . * ( 0 K.L.M.>.}.N.O.r.4 4 P.B.3.Q.R.S.T.@.5.5.5.m.v.8..   ",
"    . = _ a q U.Z V.W.X.k.4 A.l.B.t.~.4.Y.Z.@.@.5.5.5.m.v.8..   ",
"    . - : b r `. +.+++c.l.s.B.d.3.~.4.e.e.@+@.5.5.5.5.m.v.8..   ",
"    . ; < c s K #+).$+d.d.t.C.3.%+4.e.e.e.&+5.$.5.5.5.m.v.8..   ",
"    . > [ *+t L =+-+3.3.3.~.~.;+R.Y.e.e.&+&+&+5.5.5.5.>+n.8..   ",
"    . , ,+'+u @.)+~.~.4.4.4.D.I.!+~+{+&+&+&+&+&+5.5.5.f.g.8..   ",
"    . ' ]+f ' M @.{.4.4.e.E.^+/+(+@.@.5.&+&+&+5.5.5._+6.7.8..   ",
"      . 1 :+<+#.#.#.#.e.@.@.@.@.@.5.5.5.5.&+5.5.5.[+}+].8..     ",
"        . h x O $.$.$.$.$.$.5.5.5.5.5.5.5.5.5.5.[+}+|+8..       ",
"          . y P %.$.$.$.5.$.5.5.5.5.5.5.5.5.5.1+}+2+8..         ",
"            . y P %.5.5.5.5.3+5.5.5.5.$.5.5._+}+|+8..           ",
"              . y 4+6.f.>+m.m.m.m.m.m.m.>+f.6.].8..             ",
"                . 8.5+6+n.v.v.v.v.v.v.v.n.6+5+8..               ",
"                  . 8.8.8.8.8.8.8.8.8.8.8.8.8..                 ",
"                    . . . . . . . . . . . . .                   ",
"                                                                "};

/* XPM */
static char * info[] = {
"48 48 423 2",
"  	c None",
". 	c #FEFEFE",
"+ 	c #FFFFFF",
"@ 	c #F1F1F1",
"# 	c #CACACA",
"$ 	c #CBCBCB",
"% 	c #EDEDED",
"& 	c #999999",
"* 	c #4B4B4B",
"= 	c #131313",
"- 	c #2E2E2E",
"; 	c #393732",
"> 	c #423F36",
", 	c #242119",
"' 	c #282828",
") 	c #626262",
"! 	c #C2C2C2",
"~ 	c #FAFAFA",
"{ 	c #2D2D2D",
"] 	c #736F61",
"^ 	c #E0CE8A",
"/ 	c #EFDD8F",
"( 	c #F4E298",
"_ 	c #F4E191",
": 	c #F3E093",
"< 	c #ECD78C",
"[ 	c #D8C78E",
"} 	c #B7AA7C",
"| 	c #9B8C5A",
"1 	c #5F5C4F",
"2 	c #B6B6B6",
"3 	c #FEFEFD",
"4 	c #A1A1A1",
"5 	c #1E1E1E",
"6 	c #C2BEAF",
"7 	c #F1DC92",
"8 	c #F5E190",
"9 	c #F7E7A5",
"0 	c #F8EAAE",
"a 	c #F9E9A7",
"b 	c #F8E795",
"c 	c #F8E796",
"d 	c #F5E49C",
"e 	c #EBDB99",
"f 	c #E2CD7F",
"g 	c #D7C480",
"h 	c #21201E",
"i 	c #838383",
"j 	c #F9F9F9",
"k 	c #DDD9CA",
"l 	c #EFD986",
"m 	c #EFDA8E",
"n 	c #F8EAAF",
"o 	c #F9F0C7",
"p 	c #FAF5DE",
"q 	c #FAF3D0",
"r 	c #FAEAA6",
"s 	c #F9E89E",
"t 	c #F8E793",
"u 	c #F7E493",
"v 	c #EFDD93",
"w 	c #E5D187",
"x 	c #DFD2A9",
"y 	c #595958",
"z 	c #939393",
"A 	c #474747",
"B 	c #BAB6A9",
"C 	c #F5E4A3",
"D 	c #F4DE87",
"E 	c #F4E7B2",
"F 	c #F9F5E3",
"G 	c #FAF6E6",
"H 	c #FBF8ED",
"I 	c #FBF9EF",
"J 	c #FAF8EC",
"K 	c #FAF3DA",
"L 	c #F9E99B",
"M 	c #F8E791",
"N 	c #F6E38F",
"O 	c #EFDD92",
"P 	c #DFCB80",
"Q 	c #D7C896",
"R 	c #333333",
"S 	c #C9C9C9",
"T 	c #B4B4B4",
"U 	c #545454",
"V 	c #F0DD9C",
"W 	c #F3DF8D",
"X 	c #F7E69F",
"Y 	c #F8EFCF",
"Z 	c #FAF5E5",
"` 	c #F9F4E2",
" .	c #FBF6E7",
"..	c #FAF5E7",
"+.	c #FAF6E9",
"@.	c #FAEDB7",
"#.	c #F9E89B",
"$.	c #F8E691",
"%.	c #F5E391",
"&.	c #E8D68B",
"*.	c #D1BC70",
"=.	c #A29C84",
"-.	c #525252",
";.	c #848484",
">.	c #989585",
",.	c #EED681",
"'.	c #F5E293",
").	c #F8E8A9",
"!.	c #FAF4E1",
"~.	c #FAF6E7",
"{.	c #F9F5E4",
"].	c #F9F5E6",
"^.	c #F7EEC3",
"/.	c #F7E7A0",
"(.	c #F7E691",
"_.	c #F7E58F",
":.	c #F1E08E",
"<.	c #DBC878",
"[.	c #C6B677",
"}.	c #2A2A27",
"|.	c #DADADA",
"1.	c #646464",
"2.	c #C1B999",
"3.	c #EFD882",
"4.	c #F6E499",
"5.	c #F8EDBA",
"6.	c #FAF5E4",
"7.	c #FBF5E6",
"8.	c #FAF5E6",
"9.	c #F9F1DB",
"0.	c #F9F4E3",
"a.	c #F8F2DB",
"b.	c #F6EBBE",
"c.	c #F6E6A0",
"d.	c #F3E290",
"e.	c #E4D07E",
"f.	c #C1AE67",
"g.	c #646159",
"h.	c #A0A0A0",
"i.	c #EFD881",
"j.	c #F6E498",
"k.	c #FAF6E8",
"l.	c #F9F5E5",
"m.	c #F8F2DE",
"n.	c #F7EFD0",
"o.	c #F5E9C2",
"p.	c #F4E5A0",
"q.	c #F7E695",
"r.	c #F3E295",
"s.	c #E6D27E",
"t.	c #C6B36A",
"u.	c #79766C",
"v.	c #7B7B7B",
"w.	c #636363",
"x.	c #B8B095",
"y.	c #FBF8EE",
"z.	c #FAF7EB",
"A.	c #F9F6E5",
"B.	c #F9F6EA",
"C.	c #F9F4E0",
"D.	c #F4EAC9",
"E.	c #F7EBB1",
"F.	c #F5E7A2",
"G.	c #F6E697",
"H.	c #F6E59B",
"I.	c #F2E093",
"J.	c #E7D480",
"K.	c #CCB96E",
"L.	c #797157",
"M.	c #949494",
"N.	c #818181",
"O.	c #898782",
"P.	c #F0DC91",
"Q.	c #F6E496",
"R.	c #F7EAAF",
"S.	c #FBF7EC",
"T.	c #F9F6E7",
"U.	c #FAF5E8",
"V.	c #FBF7ED",
"W.	c #FAEEBB",
"X.	c #F8ECB8",
"Y.	c #F4EACA",
"Z.	c #F3E6A3",
"`.	c #F6E593",
" +	c #F6E493",
".+	c #F2E090",
"++	c #E4D17E",
"@+	c #C4B269",
"#+	c #6D664D",
"$+	c #A5A5A5",
"%+	c #BFBFBF",
"&+	c #464646",
"*+	c #EDD990",
"=+	c #F2DC8A",
"-+	c #F7E8A6",
";+	c #FAF4D9",
">+	c #F8F1DB",
",+	c #F9F4E7",
"'+	c #F9EFCB",
")+	c #FAECAF",
"!+	c #F5E7B2",
"~+	c #EDE7BA",
"{+	c #F2E29A",
"]+	c #F6E591",
"^+	c #F5E393",
"/+	c #EFDC8B",
"(+	c #D9C677",
"_+	c #AB9B5B",
":+	c #201D13",
"<+	c #D5D5D5",
"[+	c #FCFCFC",
"}+	c #FDFDFC",
"|+	c #6F6F6F",
"1+	c #9B9584",
"2+	c #F1DC86",
"3+	c #F6E59D",
"4+	c #FAF2D3",
"5+	c #FBF7EA",
"6+	c #F9F4E5",
"7+	c #FAF3E5",
"8+	c #F8ECB9",
"9+	c #F6E7A7",
"0+	c #F3E7AD",
"a+	c #F1E3A4",
"b+	c #F4E395",
"c+	c #EADA91",
"d+	c #E4D07F",
"e+	c #CDBC70",
"f+	c #817959",
"g+	c #4E4E4E",
"h+	c #DEDEDD",
"i+	c #3E3E3E",
"j+	c #EEDA8D",
"k+	c #F7E8A7",
"l+	c #FAF2D2",
"m+	c #FAF7EA",
"n+	c #F9F4E6",
"o+	c #F7EED9",
"p+	c #F1E6AD",
"q+	c #F4E49D",
"r+	c #F2E4A1",
"s+	c #F1E19B",
"t+	c #F0DF92",
"u+	c #E5D48D",
"v+	c #DBC97E",
"w+	c #D9C577",
"x+	c #B9A964",
"y+	c #4F4938",
"z+	c #A7A7A7",
"A+	c #FDFDFD",
"B+	c #262626",
"C+	c #F2E097",
"D+	c #F9EBB1",
"E+	c #FAF3DB",
"F+	c #F8F1DC",
"G+	c #F7EFD8",
"H+	c #F4E9C4",
"I+	c #F3E4A1",
"J+	c #F3E296",
"K+	c #F1E195",
"L+	c #E9DA96",
"M+	c #E6D68C",
"N+	c #DAC87E",
"O+	c #DAC777",
"P+	c #C4B36B",
"Q+	c #958857",
"R+	c #353535",
"S+	c #F4F4F4",
"T+	c #E4E4E4",
"U+	c #434032",
"V+	c #F3DE87",
"W+	c #F8EBB1",
"X+	c #F9F2DB",
"Y+	c #F3EAC1",
"Z+	c #F4E9C1",
"`+	c #F3E6AD",
" @	c #EEDB8B",
".@	c #DCCB84",
"+@	c #D1C076",
"@@	c #D1BF73",
"#@	c #C9B76D",
"$@	c #9D9164",
"%@	c #43433D",
"&@	c #C2C2C1",
"*@	c #4D493C",
"=@	c #EFD782",
"-@	c #F4E9BB",
";@	c #F4E7BB",
">@	c #F1DFA1",
",@	c #F3E3A7",
"'@	c #E3D186",
")@	c #D9C679",
"!@	c #CAB871",
"~@	c #C1B372",
"{@	c #AEA475",
"]@	c #7F7A64",
"^@	c #0B0B0B",
"/@	c #7D7D7D",
"(@	c #6D6D64",
"_@	c #121211",
":@	c #383526",
"<@	c #5F5E59",
"[@	c #5A594C",
"}@	c #A9A077",
"|@	c #E8D48D",
"1@	c #E7D180",
"2@	c #DBC77C",
"3@	c #DBC97C",
"4@	c #BAA864",
"5@	c #8F8255",
"6@	c #3B3724",
"7@	c #0F0E0C",
"8@	c #494949",
"9@	c #BBBBBB",
"0@	c #35352E",
"a@	c #9D9D99",
"b@	c #B4B4B2",
"c@	c #EFEFEE",
"d@	c #DADAD8",
"e@	c #696964",
"f@	c #1D1D16",
"g@	c #928963",
"h@	c #C2B071",
"i@	c #B3A05E",
"j@	c #A99959",
"k@	c #474439",
"l@	c #DCDCDC",
"m@	c #F5F5F5",
"n@	c #050504",
"o@	c #161614",
"p@	c #55554E",
"q@	c #8D8D85",
"r@	c #D4D4D1",
"s@	c #F6F6F6",
"t@	c #CACAC4",
"u@	c #585851",
"v@	c #4D4C47",
"w@	c #80795F",
"x@	c #716D5B",
"y@	c #2A2A2A",
"z@	c #D9D9D9",
"A@	c #585858",
"B@	c #23231F",
"C@	c #C7C7C5",
"D@	c #A9A9A4",
"E@	c #3D3D35",
"F@	c #131311",
"G@	c #8D8D87",
"H@	c #C3C3C1",
"I@	c #7B7B7A",
"J@	c #5C5C5B",
"K@	c #0D0D0A",
"L@	c #3B3B3B",
"M@	c #BABABA",
"N@	c #191919",
"O@	c #525248",
"P@	c #D7D7D3",
"Q@	c #F7F7F7",
"R@	c #CFCFC8",
"S@	c #2F2F29",
"T@	c #474742",
"U@	c #71716F",
"V@	c #757571",
"W@	c #0F0F0F",
"X@	c #ECECEC",
"Y@	c #000000",
"Z@	c #080807",
"`@	c #96968E",
" #	c #DFDFDE",
".#	c #FBFBFB",
"+#	c #B3B3B1",
"@#	c #70706B",
"##	c #4F4F4E",
"$#	c #42423A",
"%#	c #434343",
"&#	c #BCBCB9",
"*#	c #AEAEA9",
"=#	c #4B4B44",
"-#	c #0F0F0E",
";#	c #92928D",
">#	c #929292",
",#	c #696968",
"'#	c #141412",
")#	c #878787",
"!#	c #75756C",
"~#	c #C3C3C2",
"{#	c #F0F0F0",
"]#	c #B9B9B1",
"^#	c #525249",
"/#	c #464644",
"(#	c #7F7F7B",
"_#	c #6D6D6D",
":#	c #050505",
"<#	c #44443B",
"[#	c #333331",
"}#	c #B3B3B0",
"|#	c #EFEFEF",
"1#	c #CFCFCE",
"2#	c #434342",
"3#	c #56564F",
"4#	c #272727",
"5#	c #161613",
"6#	c #12120F",
"7#	c #2C2C2A",
"8#	c #9E9E9A",
"9#	c #6B6B6A",
"0#	c #10100E",
"a#	c #21211D",
"b#	c #20201C",
"c#	c #383837",
"d#	c #737373",
"e#	c #141411",
"f#	c #090908",
"g#	c #0B0B09",
"h#	c #31312B",
"i#	c #33332F",
"j#	c #30302F",
"k#	c #101010",
"l#	c #4E4E45",
"m#	c #010101",
"n#	c #1A1A17",
"o#	c #0C0C0B",
"p#	c #020202",
"                                                                                                ",
"                                                                                                ",
"                                                                                                ",
"                                                                                                ",
"                                                                                                ",
"                                                                                                ",
"                                                                                                ",
"                                                  . . + + . . .                                 ",
"                                              . . . . . . . . . . . .                           ",
"                                          + . . . . . . . . . . . . . . .                       ",
"                                        . . . . . . @ # $ $ % . . . . . . .                     ",
"                                      . . . . % & * = - ; > , ' ) ! ~ . . . .                   ",
"                                    . . . . ! { ] ^ / ( _ : < [ } | 1 2 . . . .                 ",
"                                  3 . . . 4 5 6 7 8 9 0 a b c d e f g h i j . . +               ",
"                                  . . . ! 5 k l m n o p q r s t u v w x y z . . . .             ",
"                                  . . ~ A B C D E F G H I J K L M N O P Q R S . . .             ",
"                                . . . T U V W X Y Z `  ...+.G @.#.$.%.&.*.=.-.. . .             ",
"                                . . . ;.>.,.'.).!.~.` {.` ]...^./.(._.:.<.[.}.|.. . .           ",
"                                . . . 1.2.3.4.5.6.Z 7.8.9.0.a.b.c.$.$.d.e.f.g.h.. . .           ",
"                                . . . 1.2.i.j.5.{.k.k.l.` m.n.o.p.q.q.r.s.t.u.v.. . +           ",
"                                . . . w.x.i.'.5...y.z.A.B.C.D.E.F.G.H.I.J.K.L.M.. + +           ",
"                                . . . N.O.P.Q.R.Z S.T.U.V.W.X.Y.Z.`. +.+++@+#+$+. . +           ",
"                                  . . %+&+*+=+-+;+y.>+,+'+)+!+~+{+]+^+/+(+_+:+<+. . .           ",
"                                  [+}+j |+1+2+3+4+5+6+7+8+9+0+a+b+%.c+d+e+f+g+j . .             ",
"                                  [+}+}+h+i+j+k+l+m+n+o+p+q+r+s+t+u+v+w+x+y+z+. . .             ",
"                                    }+}+A+B+C+D+E+F+G+H+I+J+K+L+M+N+O+P+Q+R+S+. . .             ",
"                                    [+}+T+U+V+W+X+Y+Z+`+s+/  @.@+@@@#@$@%@4 . . .               ",
"                                      [+&@*@=@-@;@>@,@C+/ '@)@!@~@{@]@^@/@~ . . .               ",
"                                        (@_@:@<@[@}@|@1@2@3@4@5@6@7@8@9@. . + +                 ",
"                                        0@a@b@c@d@e@f@g@h@i@j@k@) l@m@. . . +                   ",
"                                        n@o@p@q@r@s@t@u@v@w@x@y@z@}+}+. . .                     ",
"                                      A@B@C@D@E@F@G@H@I@J@K@L@M@}+}+}+.                         ",
"                                      N@O@P@A+Q@R@S@T@U@V@W@X@[+[+[+                            ",
"                                      Y@Z@S@`@ #.#+#@###$#%#                                    ",
"                                    N@Y@&#*#=#-#;#>#|+,#'#)#                                    ",
"                                    Y@!#~#{#{#]#^#/#(#_#:#                                      ",
"                                    Y@Y@<#[#}#|#1#,#2#3#4#                                      ",
"                                      Y@5#6#S@7#8#|+9#Y@                                        ",
"                                      Y@Y@0#a#b#c#,#d#Y@                                        ",
"                                      e#f#5#F@g#h#i#j#k#                                        ",
"                                      l#m#e#n#o#0#Y@                                            ",
"                                            m#p#                                                ",
"                                                                                                ",
"                                                                                                ",
"                                                                                                ",
"                                                                                                ",
"                                                                                                ",
"                                                                                                "};

/* XPM */
static char * help[] = {
"32 32 63 1",
" 	c None",
".	c #000000",
"+	c #D4C6F7",
"@	c #A789F4",
"#	c #7A49F6",
"$	c #6D40DF",
"%	c #5730BB",
"&	c #24007F",
"*	c #020007",
"=	c #713FF0",
"-	c #340D98",
";	c #5418ED",
">	c #3A07BC",
",	c #3F19A2",
"'	c #3F0FBA",
")	c #541CE3",
"!	c #5F33CE",
"~	c #9878E9",
"{	c #552CBD",
"]	c #7E57E0",
"^	c #6932F5",
"/	c #380AAD",
"(	c #A281F4",
"_	c #4C0EE9",
":	c #3E0BBF",
"<	c #4F0FF2",
"[	c #3303AE",
"}	c #3707B0",
"|	c #C7B5F6",
"1	c #4B0AF1",
"2	c #3202AD",
"3	c #3C07C2",
"4	c #4F11EC",
"5	c #5B2FCC",
"6	c #4708E9",
"7	c #5312F9",
"8	c #3F0BC2",
"9	c #3703BA",
"0	c #5115EA",
"a	c #3904BF",
"b	c #490BE8",
"c	c #3203AA",
"d	c #E8E1F8",
"e	c #AE8FFE",
"f	c #DCD2F7",
"g	c #8859FE",
"h	c #4D0FEC",
"i	c #B091FE",
"j	c #8F63FE",
"k	c #3D09C2",
"l	c #7C52E5",
"m	c #A481FE",
"n	c #9771F8",
"o	c #3102A9",
"p	c #8D61FC",
"q	c #5E35C6",
"r	c #582FBF",
"s	c #B69AFD",
"t	c #3608AD",
"u	c #471BB6",
"v	c #B599FC",
"w	c #4B0EE7",
"x	c #3709AE",
"            ..........          ",
"          ..+@@##$$%%&*.        ",
"        ..+@=......-%;>&*.      ",
"       *+@,..      .*');!&.     ",
"      .+~{.          *]^;>*     ",
"      *+=/.          .(#^_&.    ",
"     .+@:*            *@^<[.    ",
"     .@#}.            .|#12.    ",
"     .@^3*            *+#^2.    ",
"     .=#45*..        .+@#^&.    ",
"     .=67=~@@*.      *+#^8*     ",
"      *96^^##=.     *+@#0&.     ",
"       *&ab^^c.    *de#4&*      ",
"        .*&2&*    *fegh&*       ",
"          ...    *degh&*        ",
"                *figh&*         ",
"               *dig4&*          ",
"              .d+j^k*           ",
"              *+il0&.           ",
"             .d|m^a*            ",
"             .|n^<o.            ",
"             .n^<o&.            ",
"              *&o&*             ",
"               ...              ",
"                                ",
"                                ",
"               ...              ",
"              *pqr*             ",
"             .p+spt.            ",
"             .uvpw&.            ",
"              *xo&*             ",
"               ...              "};


MBReturn msgbox (gchar *textmessage, gchar *title, MBType type, MBIcon itype, MBModal modal)
{
	GtkWidget *dialogwindow;
	GtkWidget *packingbox;
	GtkWidget *buttonbox;
	GtkWidget *packingbox3;
	GtkWidget *icon = NULL;
	GdkColormap *colormap;
	GdkPixmap *pixmap;
	GdkPixmap *mask;
	GtkWidget *dialogwidget;
	GtkWidget *btnok;
	GtkWidget *btncancel;
	GtkWidget *btnyes;
	GtkWidget *btnno;

	dialogwindow = gtk_window_new (GTK_WINDOW_DIALOG);
	gtk_window_set_title (GTK_WINDOW(dialogwindow), title);
	gtk_container_set_border_width (GTK_CONTAINER (dialogwindow), 10);
	gtk_window_set_position (GTK_WINDOW(dialogwindow), GTK_WIN_POS_CENTER);

	packingbox = gtk_vbox_new (FALSE, 5);
	gtk_container_add (GTK_CONTAINER(dialogwindow), packingbox);

	packingbox3 = gtk_hbox_new (FALSE, 2);
	dialogwidget = AddWidget (packingbox3, packingbox);

	if (itype == MSGBOX_STOP)
	{
		colormap = gtk_widget_get_colormap (dialogwidget);
		pixmap = gdk_pixmap_colormap_create_from_xpm_d (NULL, colormap, &mask, NULL, stop);
		if (pixmap == NULL)
			g_error ("Couldn't create replacement pixmap.");
		icon = gtk_pixmap_new (pixmap, mask);
	}
	if (itype == MSGBOX_INFO)
	{
		colormap = gtk_widget_get_colormap (dialogwidget);
		pixmap = gdk_pixmap_colormap_create_from_xpm_d (NULL, colormap, &mask, NULL, info);
		if (pixmap == NULL)
			g_error ("Couldn't create replacement pixmap.");
		icon = gtk_pixmap_new (pixmap, mask);
	}
	if (itype == MSGBOX_QUESTION)
	{
		colormap = gtk_widget_get_colormap (dialogwidget);
		pixmap = gdk_pixmap_colormap_create_from_xpm_d (NULL, colormap, &mask, NULL, help);
		if (pixmap == NULL)
			g_error ("Couldn't create replacement pixmap.");
		icon = gtk_pixmap_new (pixmap, mask);
	}

	if(icon) {
		gdk_pixmap_unref (pixmap);
		gdk_bitmap_unref (mask);
		dialogwidget = AddWidget (icon, packingbox3);
		gtk_widget_set_usize(GTK_WIDGET(icon), 40, 40);
	}

	dialogwidget = AddWidget (gtk_label_new (textmessage), packingbox3);
	dialogwidget = AddWidget (gtk_hseparator_new(), packingbox);
	buttonbox = gtk_hbutton_box_new();
	dialogwidget = AddWidget (buttonbox, packingbox);

	btnok = gtk_button_new_with_label ("OK");
	GTK_WIDGET_SET_FLAGS (btnok, GTK_CAN_DEFAULT);
	btncancel = gtk_button_new_with_label ("Cancel");
	GTK_WIDGET_SET_FLAGS (btncancel, GTK_CAN_DEFAULT);
	btnyes = gtk_button_new_with_label ("Yes");
	GTK_WIDGET_SET_FLAGS (btnyes, GTK_CAN_DEFAULT);
	btnno = gtk_button_new_with_label ("No");
	GTK_WIDGET_SET_FLAGS (btnno, GTK_CAN_DEFAULT);

	if (type == MSGBOX_OKCANCEL)
	{
		dialogwidget = AddWidget (btnok, buttonbox);
		dialogwidget = AddWidget (btncancel, buttonbox);
		gtk_widget_grab_default (btnok);
	} else if (type == MSGBOX_YESNO) {
		dialogwidget = AddWidget (btnyes, buttonbox);
		dialogwidget = AddWidget (btnno, buttonbox);
		gtk_widget_grab_default (btnno);
	} else if (type == MSGBOX_OKONLY) {
		dialogwidget = AddWidget (btnok, buttonbox);
		gtk_widget_grab_default (btnok);
	}

	gtk_signal_connect (GTK_OBJECT (btnok),
				"clicked",
				GTK_SIGNAL_FUNC (DialogOKClicked),
				dialogwindow);
	gtk_signal_connect (GTK_OBJECT (btncancel),
				"clicked",
				GTK_SIGNAL_FUNC (DialogCancelClicked),
				dialogwindow);
	gtk_signal_connect (GTK_OBJECT (btnyes),
				"clicked",
				GTK_SIGNAL_FUNC (DialogYesClicked),
				dialogwindow);
	gtk_signal_connect (GTK_OBJECT (btnno),
				"clicked",
				GTK_SIGNAL_FUNC (DialogNoClicked),
				dialogwindow);
	gtk_signal_connect (GTK_OBJECT (dialogwidget),
				"destroy",
				GTK_SIGNAL_FUNC (CloseTheApp),
				dialogwindow);

	if (modal == MSGBOX_MODAL)
	{
		gtk_window_set_modal (GTK_WINDOW (dialogwindow), TRUE);
	}
	gtk_widget_show_all (dialogwindow);
	gtk_main();
	return mb_status;
}

