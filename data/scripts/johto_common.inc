@ Common Johto HnS script stubs shared by multiple maps.

@ Time-based encounter setup used as MAP_SCRIPT_ON_RESUME callback.
SetTimeEncounters::
	callnative SetTimeBasedEncounters
	end

@ Gift-mon helpers called after givemon/givenamedmon — show fanfare + message.
@ Stubs: mon is still given; fanfare/dex registration is skipped for now.
Common_EventScript_GiftMon::
Common_EventScript_GiftMonNamed::
	return

@ Whirlpool obstacle event
EventScript_Whirlpool::
	msgbox Johto_Text_Whirlpool, MSGBOX_DEFAULT
	releaseall
	end

Johto_Text_Whirlpool:
	.string "The water is whirling\naround violently!$"

@ Rock Smash heartscale drop (Cianwood)
EventScript_RockSmashHeartscale::
	finditem ITEM_HEART_SCALE
	end

@ Route 7 / Saffron gate door trigger stub
Route7_EventScript_Door::
	end

@ Bug Catching Contest stubs
BugContest_EventScript_TimesUp::
BugContestEventScript_Judging::
	end

@ Kanto map script labels referenced by Johto event maps
Route41_EventScript_Tentacruel1::
	end

Route34IlexForestGate_EventScript_Butterfree::
	end

SeafoamIslands_B1F_Text_ArticunoBattle:
	.string "A ferocious POKéMON lurks here!$"

@ Route 104 Flower Shop stubs
Route104_PrettyPetalFlowerShop_EventScript_MintOwner::
	msgbox Route104_PrettyPetalFlowerShop_Text_MintOwner, MSGBOX_DEFAULT
	releaseall
	end

Route104_PrettyPetalFlowerShop_Text_MintOwner:
	.string "This mint is nice.$"

@ Johto Safari Zone entrance attendant stubs
@ (Reuses Route121 label namespace from HnS source)
Route121_SafariZoneGate_SafariZoneEntrance_EventScript_WelcomeAttendant::
	msgbox Route121_SafariZoneGate_SafariZoneEntrance_Text_WouldYouLikeToPlay, MSGBOX_DEFAULT
	releaseall
	end

Route121_SafariZoneGate_SafariZoneEntrance_EventScript_InfoAttendant::
	msgbox Route121_SafariZoneGate_SafariZoneEntrance_Text_ThatWillBe500Please, MSGBOX_DEFAULT
	releaseall
	end

Route121_SafariZoneGate_SafariZoneEntrance_Text_WouldYouLikeToPlay:
	.string "Would you like to play\nin the SAFARI ZONE?\p"
	.string "It will cost 500 coins\nfor 30 SAFARI BALLS.$"

Route121_SafariZoneGate_SafariZoneEntrance_Text_PlayAnotherTime:
	.string "Please come back and\nplay in the SAFARI ZONE!$"

Route121_SafariZoneGate_SafariZoneEntrance_Text_ThatWillBe500Please:
	.string "That will be\n500 coins, please.$"

Route121_SafariZoneGate_SafariZoneEntrance_Text_HereAreYourSafariBalls:
	.string "Here are your\n30 SAFARI BALLS.$"

Route121_SafariZoneGate_SafariZoneEntrance_Text_Received30SafariBalls:
	.string "You received\n30 SAFARI BALLS.$"

Route121_SafariZoneGate_SafariZoneEntrance_Text_PleaseEnjoyYourself:
	.string "Please enjoy your\ntime in the SAFARI ZONE!$"

Route121_SafariZoneGate_SafariZoneEntrance_Text_PCIsFull:
	.string "Your PC BOX is full!\nPlease come back later.$"

Route121_SafariZoneGate_SafariZoneEntrance_Text_NotEnoughMoney:
	.string "You don't have enough\nmoney to play.$"

Route121_SafariZoneGate_SafariZoneEntrance_Text_YouNeedPokeblockCase:
	.string "You'll need a POKÉBLOCK\nCASE to enter.$"
