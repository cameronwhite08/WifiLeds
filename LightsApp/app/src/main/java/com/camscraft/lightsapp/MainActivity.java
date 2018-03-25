package com.camscraft.lightsapp;

import android.content.DialogInterface;
import android.graphics.Color;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.*;

import com.flask.colorpicker.ColorPickerView;
import com.flask.colorpicker.OnColorSelectedListener;
import com.flask.colorpicker.builder.ColorPickerClickListener;
import com.flask.colorpicker.builder.ColorPickerDialogBuilder;

import java.io.Console;

import butterknife.BindView;
import butterknife.ButterKnife;
import okhttp3.ResponseBody;
import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;
import retrofit2.Retrofit;
import retrofit2.http.GET;
import retrofit2.http.Query;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    @BindView(R.id.offButton) Button offButton;
    @BindView(R.id.sineButton) Button sineButton;
    @BindView(R.id.confButton) Button confButton;
    @BindView(R.id.jugButton) Button jugButton;
    @BindView(R.id.bpmButton) Button bpmButton;
    @BindView(R.id.rainButton) Button rainButton;
    @BindView(R.id.customButton) Button setCustomButton;

    LightsService service;

    public interface LightsService {

        @GET("off")
        Call<ResponseBody> off();

        @GET("rainbow")
        Call<ResponseBody> rainbow();

        @GET("sinelon")
        Call<ResponseBody> sinelon();

        @GET("confetti")
        Call<ResponseBody> confetti();

        @GET("bpm")
        Call<ResponseBody> bpm();

        @GET("juggle")
        Call<ResponseBody> juggle();

        @GET("setleds")
        Call<ResponseBody> setColor(@Query("r") int red, @Query("g") int green, @Query("b") int blue);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        ButterKnife.bind(this);

        Retrofit retrofit = new Retrofit.Builder()
                .baseUrl("http://192.168.1.144/")
                //.baseUrl("192.168.1.144")
                .build();


        service = retrofit.create(LightsService.class);

        offButton.setOnClickListener(this);
        sineButton.setOnClickListener(this);
        confButton.setOnClickListener(this);
        jugButton.setOnClickListener(this);
        bpmButton.setOnClickListener(this);
        rainButton.setOnClickListener(this);
        setCustomButton.setOnClickListener(this);
    }

    @Override
    public void onClick(View view) {
        Call<ResponseBody> t = null;

        switch (view.getId()){
            case R.id.offButton:
                t = service.off();
                break;
            case R.id.sineButton:
                t = service.sinelon();
                break;
            case R.id.confButton:
                t = service.confetti();
                break;
            case R.id.jugButton:
                t = service.juggle();
                break;
            case R.id.bpmButton:
                t = service.bpm();
                break;
            case R.id.rainButton:
                t = service.rainbow();
                break;
            case R.id.customButton:
                ColorPickerDialogBuilder
                        .with(this)
                        .setTitle("Choose color")
                        .wheelType(ColorPickerView.WHEEL_TYPE.FLOWER)
                        .density(12)
                        .setPositiveButton("ok", new ColorPickerClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int selectedColor, Integer[] allColors) {
                                int red = Color.red( selectedColor);
                                int green = Color.green(selectedColor);
                                int blue = Color.blue(selectedColor);

                                final Call<ResponseBody> s = service.setColor(red, green, blue);
                                s.enqueue(new Callback<ResponseBody>() {
                                    @Override
                                    public void onResponse(Call<ResponseBody> call, Response<ResponseBody> response) {

                                    }

                                    @Override
                                    public void onFailure(Call<ResponseBody> call, Throwable t) {

                                    }
                                });
                            }
                        })
                        .build()
                        .show();
                break;
        }


        if(t != null){

            t.enqueue(new Callback<ResponseBody>() {
                @Override
                public void onResponse(Call<ResponseBody> call, Response<ResponseBody> response) {

                }

                @Override
                public void onFailure(Call<ResponseBody> call, Throwable t) {

                }
            });

        }
    }
}
