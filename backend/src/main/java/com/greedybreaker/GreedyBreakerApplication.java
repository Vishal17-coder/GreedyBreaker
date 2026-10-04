package com.greedybreaker;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.boot.context.properties.EnableConfigurationProperties;

@SpringBootApplication
@EnableConfigurationProperties
public class GreedyBreakerApplication {

    public static void main(String[] args) {
        SpringApplication.run(GreedyBreakerApplication.class, args);
    }
}
