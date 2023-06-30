#version 330

uniform sampler2D tex;
uniform sampler2D tex1;
uniform bool useTexture;
uniform vec3 objectColor;

out vec4 pixelColor; //Zmienna wyjsciowa fragment shadera. Zapisuje sie do niej ostateczny (prawie) kolor piksela

//Zmienne interpolowane
in vec4 n;
in vec4 l1;
in vec4 l2;
in vec4 v;
in vec2 iTexCoord0; //globalnie

void main(void) {
if(useTexture) {
	//Znormalizowane interpolowane wektory
	vec4 ml = normalize(l1+l2);
	vec4 mn = normalize(n);
	vec4 mv = normalize(v);

	//Wektor odbity
	vec4 mr = reflect(-ml, mn);
	vec4 texColor=texture(tex,iTexCoord0);
	vec4 texColor1=texture(tex1,iTexCoord0);

	//Parametry powierzchni
	vec4 kd=texColor;
	vec4 ks = texColor1;
	//vec4 ks = vec4(1,1,1,1);

	//Obliczenie modelu oœwietlenia
	float nl = 1-clamp(dot(mn, ml), 0, 1);
	float rv = pow(clamp(dot(mr, mv), 0, 1), 50);
	pixelColor= vec4(kd.rgb * nl, kd.a) + vec4(ks.rgb*rv, 0);
}
else {
	vec3 color=texture(tex,iTexCoord0).rgb;
	pixelColor = vec4(color,1.0);
}
}