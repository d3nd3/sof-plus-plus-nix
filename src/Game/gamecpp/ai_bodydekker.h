

class bodydekker_c : public bodymeso_c
{
protected:
	int			totalDamage;//total amount of damage taken, absorbed by armor or no
	float		lastDekkerPain;//last time dekker tried to play a pain anim--avoid doing too often
public:
						bodydekker_c();
	virtual	int			ShowDamage (edict_t &monster, edict_t *inflictor, edict_t *attacker, vec3_t dir, vec3_t point, vec3_t origin, int damage, int knockback, int dflags, int mod, float penetrate, float absorb);
	virtual int			GetClassCode(void){return BODY_DEKKER;}//this should be different for everybody in this class family

	virtual	mmove_t		*GetSequenceForPain(edict_t &monster, vec3_t point, float kick, int damage, vec3_t dest=vec3_origin, vec3_t face=vec3_origin, actionsub_code seqtype=ACTSUB_NORMAL, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int reject_actionflags=0);

						bodydekker_c(bodydekker_c *orig);
	virtual void		Evaluate(bodydekker_c *orig);
	virtual void		Write();
	virtual void		Read();
};

