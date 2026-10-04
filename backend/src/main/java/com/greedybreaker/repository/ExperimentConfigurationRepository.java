package com.greedybreaker.repository;

import com.greedybreaker.model.ExperimentConfiguration;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

@Repository
public interface ExperimentConfigurationRepository extends JpaRepository<ExperimentConfiguration, String> {
}
