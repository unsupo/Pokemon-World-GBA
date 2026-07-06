@ Elite Four badge gate system.
@ Each regional E4 entrance calls EventScript_CheckEliteFourEntry.
@ Requires: 8 badges for 1st E4, 16 for 2nd, 24 for 3rd (across all regions).
@
@ After calling, branch on VAR_RESULT: 0=blocked, 1=allowed.
@ gSpecialVar_0x8001 holds required badge count for the blocked message.
@ gSpecialVar_0x8002 holds the player's current badge count.

EventScript_CheckEliteFourEntry::
	special Special_CheckEliteFourEligibility
	compare VAR_RESULT, 0
	goto_if_eq EventScript_EliteFourBlocked
	return

EventScript_EliteFourBlocked::
	lock
	faceplayer
	@ Build the "you need X badges" message from the vars.
	@ VAR 0x8001 = required, VAR 0x8002 = actual.
	@ This calls a shared text script that formats the message.
	call EventScript_EliteFourBlockedMessage
	releaseall
	end

EventScript_EliteFourBlockedMessage::
	@ Simple fallback: a single text box.
	@ Replace with a more informative message once text compression is set up.
	msgbox EliteFour_Text_NeedMoreBadges, MSGBOX_DEFAULT
	return

EliteFour_Text_NeedMoreBadges:
	.string "You can't enter yet!\p"
	.string "The Elite Four respects only\l"
	.string "trainers who have proven\l"
	.string "themselves across all regions.\p"
	.string "Earn more badges and come\l"
	.string "back when you're ready!$"
