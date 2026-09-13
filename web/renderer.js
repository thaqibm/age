export function makeRenderer(canvas) {
  let gl, program, buffer;
  gl = canvas.getContext("webgl2", {
    alpha: true,
    antialias: false,
    preserveDrawingBuffer: false,
  });
  if (!gl)
    throw new Error(
      "WebGL 2 is unavailable. Try a browser with hardware acceleration enabled.",
    );
  const shader = (type, source) => {
    const s = gl.createShader(type);
    gl.shaderSource(s, source);
    gl.compileShader(s);
    if (!gl.getShaderParameter(s, gl.COMPILE_STATUS))
      throw new Error(gl.getShaderInfoLog(s));
    return s;
  };
  program = gl.createProgram();
  gl.attachShader(
    program,
    shader(
      gl.VERTEX_SHADER,
      `#version 300 es
 precision highp float;
 layout(location=0) in vec4 rect;
 layout(location=1) in vec4 colorShape;
 layout(location=2) in vec3 extra;
 out vec2 uv;out vec4 color;flat out int shape;
 void main(){vec2 corners[6]=vec2[6](vec2(-1,-1),vec2(1,-1),vec2(-1,1),vec2(-1,1),vec2(1,-1),vec2(1,1));uv=corners[gl_VertexID];vec2 q=uv*rect.zw;float c=cos(extra.x),s=sin(extra.x);vec2 p=rect.xy+mat2(c,s,-s,c)*q;gl_Position=vec4(p.x/480.-1.,1.-p.y/300.,0,1);color=vec4(colorShape.rgb,extra.y);shape=int(colorShape.a);}`,
    ),
  );
  gl.attachShader(
    program,
    shader(
      gl.FRAGMENT_SHADER,
      `#version 300 es
 precision highp float;
 in vec2 uv;in vec4 color;flat in int shape;out vec4 outColor;
 void main(){float edge=0.;if(shape==0)edge=length(uv)-1.;else if(shape==1)edge=abs(uv.x)+abs(uv.y)-1.;else if(shape==2)edge=max(abs(uv.x)-(uv.y+1.)*.5,-uv.y-1.);else edge=max(abs(uv.x),abs(uv.y))-1.;float a=1.-smoothstep(-.08,0.,edge);if(a<=0.)discard;outColor=vec4(color.rgb,color.a*a);}`,
    ),
  );
  gl.linkProgram(program);
  if (!gl.getProgramParameter(program, gl.LINK_STATUS))
    throw new Error(gl.getProgramInfoLog(program));
  gl.useProgram(program);
  buffer = gl.createBuffer();
  gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
  gl.bufferData(gl.ARRAY_BUFFER, 5000 * 44, gl.DYNAMIC_DRAW);
  for (const [i, n, offset] of [
    [0, 4, 0],
    [1, 4, 16],
    [2, 3, 32],
  ]) {
    gl.enableVertexAttribArray(i);
    gl.vertexAttribPointer(i, n, gl.FLOAT, false, 44, offset);
    gl.vertexAttribDivisor(i, 1);
  }
  gl.enable(gl.BLEND);
  gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);
  gl.clearColor(0, 0, 0, 0);
  return function render(engine) {
    const ratio = Math.min(devicePixelRatio || 1, 2),
      w = Math.round(canvas.clientWidth * ratio),
      h = Math.round(canvas.clientHeight * ratio);
    if (canvas.width !== w || canvas.height !== h) {
      canvas.width = w;
      canvas.height = h;
      gl.viewport(0, 0, w, h);
    }
    gl.clear(gl.COLOR_BUFFER_BIT);
    const n = engine._game_count(),
      offset = engine._game_vertices() / 4;
    gl.bufferSubData(
      gl.ARRAY_BUFFER,
      0,
      engine.HEAPF32.subarray(offset, offset + n * 11),
    );
    gl.drawArraysInstanced(gl.TRIANGLES, 0, 6, n);
  };
}
