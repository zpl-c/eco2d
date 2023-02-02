void spritesheet_viewer(struct nk_context *ctx, struct nk_image spritesheet, Vector2 frameSize, int framesPerRow) {
	const int maxFrames = (int)((spritesheet.w*spritesheet.h) / (frameSize.x*frameSize.y));
	nk_layout_row_static(ctx, 32, 32, (int)(nk_window_get_size(ctx).x / frameSize.x) -1);
	for(int frame = 0; frame < maxFrames; frame++) {
		float ox = (frame % framesPerRow) * frameSize.x;
		float oy = (int)(frame / framesPerRow) * frameSize.y;
		spritesheet.region[0] = (nk_ushort)ox;
		spritesheet.region[1] = (nk_ushort)oy;
		spritesheet.region[2] = (nk_ushort)frameSize.x;
		spritesheet.region[3] = (nk_ushort)frameSize.y;
		nk_image(ctx, spritesheet);
		nk_labelf(ctx, NK_TEXT_ALIGN_LEFT, "%d", frame);
	}
}
