#include <nanovg_gl.h>

NVGcontext* nvgCreateGL3(int flags) {
    NVGparams params;
    NVGcontext* ctx = NULL;
    GLNVGcontext* gl = (GLNVGcontext*)malloc(sizeof(GLNVGcontext));
    if (gl == NULL) goto error;
    memset(gl, 0, sizeof(GLNVGcontext));

    memset(&params, 0, sizeof(params));
    params.renderCreate = glnvg__renderCreate;
    params.renderCreateTexture = glnvg__renderCreateTexture;
    params.renderDeleteTexture = glnvg__renderDeleteTexture;
    params.renderUpdateTexture = glnvg__renderUpdateTexture;
    params.renderGetTextureSize = glnvg__renderGetTextureSize;
    params.renderViewport = glnvg__renderViewport;
    params.renderCancel = glnvg__renderCancel;
    params.renderFlush = glnvg__renderFlush;
    params.renderFill = glnvg__renderFill;
    params.renderStroke = glnvg__renderStroke;
    params.renderTriangles = glnvg__renderTriangles;
    params.renderDelete = glnvg__renderDelete;
    params.userPtr = gl;
    params.edgeAntiAlias = flags & NVG_ANTIALIAS ? 1 : 0;

    gl->flags = flags;

    ctx = nvgCreateInternal(&params);
    if (ctx == NULL) goto error;

    return ctx;

error:
    // 'gl' is freed by nvgDeleteInternal.
    if (ctx != NULL) nvgDeleteInternal(ctx);
    return NULL;
}

void nvgDeleteGL3(NVGcontext* ctx) { nvgDeleteInternal(ctx); }

int nvglCreateImageFromHandleGL3(NVGcontext* ctx, GLuint textureId, int w, int h, int imageFlags) {
    GLNVGcontext* gl = (GLNVGcontext*)nvgInternalParams(ctx)->userPtr;
    GLNVGtexture* tex = glnvg__allocTexture(gl);

    if (tex == NULL) return 0;

    tex->type = NVG_TEXTURE_RGBA;
    tex->tex = textureId;
    tex->flags = imageFlags;
    tex->width = w;
    tex->height = h;

    return tex->id;
}

GLuint nvglImageHandleGL3(NVGcontext* ctx, int image) {
    GLNVGcontext* gl = (GLNVGcontext*)nvgInternalParams(ctx)->userPtr;
    GLNVGtexture* tex = glnvg__findTexture(gl, image);
    return tex->tex;
}
